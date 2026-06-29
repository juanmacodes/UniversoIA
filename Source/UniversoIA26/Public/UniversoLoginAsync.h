#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "UniversoLoginAsync.generated.h"

/** Datos del usuario autenticado contra el backend (Supabase auth.users + public.users). */
USTRUCT(BlueprintType)
struct FUniversoUser
{
	GENERATED_BODY()

	/** UUID del usuario en auth.users. */
	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString UserId;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString Email;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString FirstName;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString LastName;

	/** Nombre para mostrar: "FirstName LastName" o, si falta, el email. */
	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString Role;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString Status;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString InstitutionId;

	/** JWT de acceso devuelto por Supabase (para llamadas autenticadas posteriores). */
	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString AccessToken;

	UPROPERTY(BlueprintReadOnly, Category = "Universo|Auth")
	FString RefreshToken;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUniversoLoginSuccess, const FUniversoUser&, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUniversoLoginFailure, const FString&, ErrorMessage);

/**
 * Nodo asincrono de login contra el backend de Universo IA (Supabase).
 * Flujo: POST /auth/v1/token?grant_type=password  ->  GET /rest/v1/users (perfil).
 * SupabaseUrl y AnonKey pueden venir por parametro o, si se dejan vacios, se leen
 * de DefaultGame.ini en la seccion [UniversoBackend] (claves SupabaseUrl / SupabaseAnonKey).
 */
UCLASS()
class UNIVERSOIA26_API UUniversoLoginAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Se dispara cuando las credenciales son correctas; devuelve el usuario y su info. */
	UPROPERTY(BlueprintAssignable)
	FUniversoLoginSuccess OnSuccess;

	/** Se dispara si las credenciales son incorrectas o hay un error de red/servidor. */
	UPROPERTY(BlueprintAssignable)
	FUniversoLoginFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Login Universo IA (Supabase)", AdvancedDisplay = "SupabaseUrl,AnonKey"), Category = "Universo|Auth")
	static UUniversoLoginAsync* LoginWithEmailPassword(
		UObject* WorldContextObject,
		const FString& Email,
		const FString& Password,
		const FString& SupabaseUrl,
		const FString& AnonKey);

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	FString Email;
	FString Password;
	FString SupabaseUrl;
	FString AnonKey;

	FUniversoUser User;

	void HandleAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void FetchProfile();
	void HandleProfileResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void FinishSuccess();
	void BroadcastFailure(const FString& Message);

	static FString NormalizeBaseUrl(const FString& In);
};
