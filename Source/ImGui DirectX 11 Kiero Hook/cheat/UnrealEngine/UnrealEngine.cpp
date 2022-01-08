#include "UnrealClasses.h"
#include "utils.h"



bool AEnemy::IsVisible() const
{
	vec3 dst = this->GetBonePos(48/*head*/);

	FHitResult hit;
	TraceLine<2>(CameraManager->Location,							// 3d world source
		dst,														// 3d world destination
		hit,														// out hit info
		{ (void*)this, LocalPlayer->PlayerController->Character });	// Actors to ignore											

	if (hit.bBlockingHit)
	{
		AEnemy* actor = hit.GetActor();
		if (actor && std::string(mem::GetGName(actor->FNameIndex))._Starts_with("Glass")) // maybe we hit glass
		{
			if (TraceLine<1>(hit.ImpactPoint, dst, hit, { (void*)actor }, 1)) // pass in 1 as the trace channel because for whatever reason the Glass actor on the 0 trace channel doesn't get ignored although I pass it as an argument to the function
			{
				actor = hit.GetActor();
				if (actor->IsPlayer())
					return true;
			}
		}
		return false;
	}
	return true;
}

AEnemy* FHitResult::GetActor()
{
	for (USHORT i = 0; i < EntityList->count; i++)
		if (mem::GetPointerSafe((uintptr_t)EntityList->items[i] + 0x18, { 0x0 }) &&
			EntityList->items[i]->ObjectIndex == this->ActorSerialNumber + 3) // add 3 because for some reason the ActorSerialNumber is almost always offseted by 3; if anyone knows why and how to maybe "fix" it, I would appricate if you made a pull request
			return EntityList->items[i];

	return nullptr;
}

vec3 AEnemy::GetVelocity() const
{
	return this->RootComp->Velocity;
}

vec3 PlayerBP_C::GetVelocity() const
{
	return this->RootComp->Velocity;
}
