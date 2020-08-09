#pragma once


#define PLAYERCHAR_MOVEMENT_FUNC_VR		0x05E07A0

class ICollider
{
public:
	ICollider() = default;
	virtual ~ICollider() = default;

	void PreCollide(not_null<RE::Actor*> a_actor, RE::TESObjectREFR* a_colRef);
	void PostCollide();

protected:
	virtual bool ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef) = 0;

private:
	void SetCollisionObject(not_null<RE::Actor*> a_actor);
	void ClearCollisionObject();
	void SetCollisionOnObject(bool a_set);

	RE::hkRefPtr<RE::hkpWorldObject> _collisionObj;
};


class CollisionHandler
{
public:
	static void Install();

private:
	static bool Init();

	static void Hook_ApplyMovementDelta(not_null<RE::Actor*> a_actor, float a_delta);

	static bool CanProcess(not_null<RE::Actor*> a_actor, float a_delta);
	static void PreCollide(not_null<RE::Actor*> a_actor, RE::TESObjectREFR* a_colRef);
	static void PostCollide();

	static inline REL::Offset<decltype(Hook_ApplyMovementDelta)> _ApplyMovementDelta;
	static inline std::vector<std::unique_ptr<ICollider>> _colliders;
};


class DialogueCollider : public ICollider
{
private:
	using super = ICollider;

public:
	DialogueCollider() = default;
	~DialogueCollider() = default;

protected:
	bool ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef) override;
};


class AllyCollider : public ICollider
{
private:
	using super = ICollider;

public:
	AllyCollider() = default;
	~AllyCollider() = default;

protected:
	bool ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef) override;
};


class SummonCollider : public ICollider
{
private:
	using super = ICollider;

public:
	SummonCollider() = default;
	~SummonCollider() = default;

protected:
	bool ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef) override;
};


class AllySummonCollider : public ICollider
{
private:
	using super = ICollider;

public:
	AllySummonCollider() = default;
	~AllySummonCollider() = default;

protected:
	bool ShouldIgnoreCollision(RE::TESObjectREFR* a_colRef) override;
};
