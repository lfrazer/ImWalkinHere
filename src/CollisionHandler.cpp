#include "CollisionHandler.h"

#include "Events.h"
#include "Settings.h"

#ifdef SKYRIMVR
#include "SKSE/Interfaces.h"
#include "SKSE/API.h"
#include "RE/AIProcess.h"
#endif

constexpr auto COLLISION_FLAG = static_cast<UInt32>(RE::CFilter::Flag::kNoCollision);


void ICollider::PreCollide(not_null<RE::Actor*> a_actor, RE::TESObjectREFR* a_colRef)
{
	if (ShouldIgnoreCollision(a_colRef)) {
		SetCollisionObject(a_actor);
		SetCollisionOnObject(false);
	}
}


void ICollider::PostCollide()
{
	if (_collisionObj) {
		SetCollisionOnObject(true);
		ClearCollisionObject();
	}
}


void ICollider::SetCollisionObject(not_null<RE::Actor*> a_actor)
{
	auto controller = a_actor->GetCharController();
	_collisionObj = controller->bumpedCharCollisionObject;
}


void ICollider::ClearCollisionObject()
{
	_collisionObj.reset();
}


void ICollider::SetCollisionOnObject(bool a_set)
{
	auto& filter = _collisionObj->collidable.broadPhaseHandle.collisionFilterInfo;
	if (!a_set) {
		filter |= COLLISION_FLAG;
	} else {
		filter &= ~COLLISION_FLAG;
	}
}


void CollisionHandler::Install()
{
	if (!Init()) {
		_ERROR("%s did not have any collision handlers", typeid(CollisionHandler).name());
		return;
	}

#ifdef SKYRIMVR
	REL::Offset<std::uintptr_t> hookPoint{ REL::Offset<std::uintptr_t>( (std::uintptr_t)(PLAYERCHAR_MOVEMENT_FUNC_VR + 0xF0)) };
	uintptr_t hookAddr = hookPoint.GetAddress();
#else
	REL::Offset<std::uintptr_t> hookPoint{ REL::ID(36359), 0xF0 };
	uintptr_t hookAddr = hookPoint.address();
#endif

	auto trampoline = SKSE::GetTrampoline();
	_ApplyMovementDelta = (decltype(_ApplyMovementDelta))trampoline->Write5CallEx(hookAddr, Hook_ApplyMovementDelta);

	_MESSAGE("Installed hooks for %s", typeid(CollisionHandler).name());
}


bool CollisionHandler::Init()
{
	if (*Settings::disableDialogueCollision) {
		_colliders.push_back(std::make_unique<DialogueCollider>());
	}

	if (*Settings::disableAllyCollision) {
		_colliders.push_back(std::make_unique<AllyCollider>());
	}

	if (*Settings::disableSummonCollision) {
		_colliders.push_back(std::make_unique<SummonCollider>());
	}

	if (*Settings::disableAllySummonCollision) {
		_colliders.push_back(std::make_unique<AllySummonCollider>());
	}

	return !_colliders.empty();
}


void CollisionHandler::Hook_ApplyMovementDelta(not_null<RE::Actor*> a_actor, float a_delta)
{
	if (!CanProcess(a_actor, a_delta)) {
		_ApplyMovementDelta(a_actor, a_delta);
	}
}


bool CollisionHandler::CanProcess(not_null<RE::Actor*> a_actor, float a_delta)
{
	if (!a_actor->IsPlayerRef()) {
		return false;
	}

	auto controller = a_actor->GetCharController();
	if (!controller) {
		return false;
	}

	auto collisionObj = controller->bumpedCharCollisionObject.get();
	if (!collisionObj) {
		return false;
	}

	auto& filter = collisionObj->collidable.broadPhaseHandle.collisionFilterInfo;
	if (filter & COLLISION_FLAG) {
		return false;
	}

	auto colRef = RE::TESHavokUtilities::FindCollidableRef(collisionObj->collidable);
	PreCollide(a_actor, colRef);

	_ApplyMovementDelta(a_actor, a_delta);

	PostCollide();

	return true;
}


void CollisionHandler::PreCollide(not_null<RE::Actor*> a_actor, RE::TESObjectREFR* a_colRef)
{
	for (auto& collider : _colliders) {
		collider->PreCollide(a_actor, a_colRef);
	}
}


void CollisionHandler::PostCollide()
{
	for (auto& collider : _colliders) {
		collider->PostCollide();
	}
}


bool DialogueCollider::ShouldIgnoreCollision(RE::TESObjectREFR*)
{
	auto menuHandler = Events::MenuOpenCloseHandler::GetSingleton();
	return menuHandler->IsInDialogue();
}


bool AllyCollider::ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef)
{
	if (!a_colRef || a_colRef->IsNot(RE::FormType::ActorCharacter)) {
		return false;
	}

	auto colActor = static_cast<RE::Actor*>(a_colRef);
	return colActor->IsPlayerTeammate() && !colActor->IsAMount();
}


bool SummonCollider::ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef)
{
	if (!a_colRef || a_colRef->IsNot(RE::FormType::ActorCharacter)) {
		return false;
	}

	auto player = RE::PlayerCharacter::GetSingleton();
	auto colActor = static_cast<RE::Actor*>(a_colRef);
	if (!player || colActor->IsAMount()) {
		return false;
	}
	
	// early out for now..
	return false;

	// TODO: Fix linking issues here??
	/*
#ifdef SKYRIMVR
	auto hCommander = colActor->currentProcess->GetCommandingActor();
#else
	auto hCommander = colActor->GetCommandingActor();
#endif
	auto hPlayer = player->CreateRefHandle();
	return hCommander && hPlayer && hCommander == hPlayer;
	*/
}


bool AllySummonCollider::ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef)
{
	if (!a_colRef || a_colRef->IsNot(RE::FormType::ActorCharacter)) {
		return false;
	}

	auto player = RE::PlayerCharacter::GetSingleton();
	auto colActor = static_cast<RE::Actor*>(a_colRef);
	if (!player | colActor->IsAMount()) {
		return false;
	}

	// early out for now..
	return false;

	// TODO: Fix linking issues here??
	/*
#ifdef SKYRIMVR
	auto hCommander = colActor->currentProcess->GetCommandingActor();
#else
	auto hCommander = colActor->GetCommandingActor();
#endif
	auto commander = hCommander.get();
	return commander && commander->IsPlayerTeammate();
	*/
}
