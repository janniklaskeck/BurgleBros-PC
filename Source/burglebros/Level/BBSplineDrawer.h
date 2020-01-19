// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBRoomInfo.h"
#include "Materials/Material.h"
#include "BBSplineDrawer.generated.h"

UCLASS()
class BURGLEBROS_API ABBSplineDrawer : public AActor
{
	GENERATED_BODY()

public:
	ABBSplineDrawer();

protected:
	virtual void BeginPlay() override;

	class USceneComponent* Root;
	UPROPERTY(Category = Navigation, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GuardReachableIndicatorMesh;
	UPROPERTY(Category = Navigation, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* GuardFinalIndicatorMesh;
	UPROPERTY(Category = Navigation, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* PlayerReachIndicatorMesh;

	class USplineComponent* PlayerSplineComponent;
	TArray<class USplineMeshComponent*> PlayerSplineMeshComponents;
	TArray<class USplineComponent*> GuardSplineComponents;
	TArray<TArray<class USplineMeshComponent*>> GuardSplineMeshComponents;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		UMaterialInterface* GuardMeshMaterial;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		UMaterialInterface* PlayerMeshMaterial;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMesh* SplineStaticMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		FVector SplineOffset;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		FVector IndicatorOffset;

public:
	void DrawGuardPath(int32 Floor);
	void DrawPlayerPath();
private:
	void DrawPath(TArray<FRoomPosition> Path, class USplineComponent* SplineComponent, TArray<USplineMeshComponent*>& SplineMeshComponents, UMaterialInterface* Material);
	void ClearPath(class USplineComponent* SplineComponent, TArray<USplineMeshComponent*>& SplineMeshComponents);
public:
	void ClearGuardPath(int32 Floor);
	void ClearPlayerPath();
};
