#include "UniversoLoginAsync.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Misc/ConfigCacheIni.h"

UUniversoLoginAsync* UUniversoLoginAsync::LoginWithEmailPassword(
	UObject* InWorldContextObject,
	const FString& InEmail,
	const FString& InPassword,
	const FString& InSupabaseUrl,
	const FString& InAnonKey)
{
	UUniversoLoginAsync* Node = NewObject<UUniversoLoginAsync>();
	Node->WorldContextObject = InWorldContextObject;
	Node->Email = InEmail;
	Node->Password = InPassword;
	Node->SupabaseUrl = InSupabaseUrl;
	Node->AnonKey = InAnonKey;
	return Node;
}

FString UUniversoLoginAsync::NormalizeBaseUrl(const FString& In)
{
	FString Out = In.TrimStartAndEnd();
	while (Out.EndsWith(TEXT("/")))
	{
		Out = Out.LeftChop(1);
	}
	return Out;
}

void UUniversoLoginAsync::Activate()
{
	// Fallback a DefaultGame.ini [UniversoBackend] si no se pasaron por parametro.
	if (SupabaseUrl.IsEmpty() && GConfig)
	{
		GConfig->GetString(TEXT("UniversoBackend"), TEXT("SupabaseUrl"), SupabaseUrl, GGameIni);
	}
	if (AnonKey.IsEmpty() && GConfig)
	{
		GConfig->GetString(TEXT("UniversoBackend"), TEXT("SupabaseAnonKey"), AnonKey, GGameIni);
	}

	SupabaseUrl = NormalizeBaseUrl(SupabaseUrl);

	if (SupabaseUrl.IsEmpty() || AnonKey.IsEmpty())
	{
		BroadcastFailure(TEXT("Falta configuracion de backend: define SupabaseUrl y SupabaseAnonKey (parametros del nodo o seccion [UniversoBackend] en DefaultGame.ini)."));
		return;
	}

	if (Email.IsEmpty() || Password.IsEmpty())
	{
		BroadcastFailure(TEXT("Introduce correo y contrasena."));
		return;
	}

	const FString Url = FString::Printf(TEXT("%s/auth/v1/token?grant_type=password"), *SupabaseUrl);

	TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
	Body->SetStringField(TEXT("email"), Email);
	Body->SetStringField(TEXT("password"), Password);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	if (!FJsonSerializer::Serialize(Body.ToSharedRef(), Writer))
	{
		BroadcastFailure(TEXT("No se pudo serializar la peticion de login."));
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("apikey"), AnonKey);
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UUniversoLoginAsync::HandleAuthResponse);

	if (!Request->ProcessRequest())
	{
		BroadcastFailure(TEXT("No se pudo lanzar la peticion HTTP de login."));
	}
}

void UUniversoLoginAsync::HandleAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		BroadcastFailure(TEXT("No hay conexion con el servidor de autenticacion."));
		return;
	}

	const FString Content = Response->GetContentAsString();
	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
	const bool bParsed = FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid();

	const int32 Code = Response->GetResponseCode();
	if (Code < 200 || Code >= 300)
	{
		FString Msg = TEXT("Credenciales incorrectas.");
		if (bParsed)
		{
			FString Desc;
			if (Root->TryGetStringField(TEXT("error_description"), Desc) && !Desc.IsEmpty()) { Msg = Desc; }
			else if (Root->TryGetStringField(TEXT("msg"), Desc) && !Desc.IsEmpty()) { Msg = Desc; }
			else if (Root->TryGetStringField(TEXT("error"), Desc) && !Desc.IsEmpty()) { Msg = Desc; }
		}
		BroadcastFailure(Msg);
		return;
	}

	if (!bParsed)
	{
		BroadcastFailure(TEXT("Respuesta de login no valida."));
		return;
	}

	Root->TryGetStringField(TEXT("access_token"), User.AccessToken);
	Root->TryGetStringField(TEXT("refresh_token"), User.RefreshToken);

	const TSharedPtr<FJsonObject>* UserObj = nullptr;
	if (Root->TryGetObjectField(TEXT("user"), UserObj) && UserObj && UserObj->IsValid())
	{
		(*UserObj)->TryGetStringField(TEXT("id"), User.UserId);
		(*UserObj)->TryGetStringField(TEXT("email"), User.Email);
	}

	if (User.AccessToken.IsEmpty() || User.UserId.IsEmpty())
	{
		BroadcastFailure(TEXT("El servidor no devolvio un token de sesion valido."));
		return;
	}

	FetchProfile();
}

void UUniversoLoginAsync::FetchProfile()
{
	const FString Select = TEXT("email,first_name,last_name,role,status,institution_id");
	const FString Url = FString::Printf(
		TEXT("%s/rest/v1/users?id=eq.%s&select=%s"),
		*SupabaseUrl,
		*FGenericPlatformHttp::UrlEncode(User.UserId),
		*FGenericPlatformHttp::UrlEncode(Select));

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("apikey"), AnonKey);
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *User.AccessToken));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->OnProcessRequestComplete().BindUObject(this, &UUniversoLoginAsync::HandleProfileResponse);

	if (!Request->ProcessRequest())
	{
		// El login fue correcto; si no podemos pedir el perfil, devolvemos lo que tenemos.
		FinishSuccess();
	}
}

void UUniversoLoginAsync::HandleProfileResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		const int32 Code = Response->GetResponseCode();
		if (Code >= 200 && Code < 300)
		{
			const FString Content = Response->GetContentAsString();
			TArray<TSharedPtr<FJsonValue>> Rows;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
			if (FJsonSerializer::Deserialize(Reader, Rows) && Rows.Num() > 0)
			{
				const TSharedPtr<FJsonObject> Obj = Rows[0]->AsObject();
				if (Obj.IsValid())
				{
					FString Tmp;
					if (Obj->TryGetStringField(TEXT("email"), Tmp) && !Tmp.IsEmpty()) { User.Email = Tmp; }
					Obj->TryGetStringField(TEXT("first_name"), User.FirstName);
					Obj->TryGetStringField(TEXT("last_name"), User.LastName);
					Obj->TryGetStringField(TEXT("role"), User.Role);
					Obj->TryGetStringField(TEXT("status"), User.Status);
					Obj->TryGetStringField(TEXT("institution_id"), User.InstitutionId);
				}
			}
		}
	}

	FinishSuccess();
}

void UUniversoLoginAsync::FinishSuccess()
{
	FString Name = (User.FirstName + TEXT(" ") + User.LastName).TrimStartAndEnd();
	User.DisplayName = Name.IsEmpty() ? User.Email : Name;

	OnSuccess.Broadcast(User);
	SetReadyToDestroy();
}

void UUniversoLoginAsync::BroadcastFailure(const FString& Message)
{
	OnFailure.Broadcast(Message);
	SetReadyToDestroy();
}
