#include "NewProjectiles.h"
#include "FenixProjectilesAPI.h"

bool is_CustomPosType(RE::Projectile* proj)
{
	return proj->pad164 == FenixProjs::CustomPosType();
}

void set_CustomPosType(RE::Projectile* proj) { proj->pad164 = FenixProjs::CustomPosType(); }

void set_NormalType(RE::Projectile* proj) { proj->pad164 = FenixProjs::NormalType(); }

struct Projectile__LaunchData
{
	void* vftable_LaunchData_0;
	RE::NiPoint3 startPos;
	RE::NiPoint3 Point_14;
	RE::BGSProjectile* projectile;
	RE::TESObjectREFR* source;
	RE::CombatController* combatController;
	RE::TESObjectWEAP* weap;
	RE::TESAmmo* overwriteAmmo;
	float rotationZ;
	float rotationX;
	void* field_50;
	RE::TESObjectREFR* target;
	std::pair<float, float> drawn_time;
	RE::TESObjectCELL* cell;
	RE::MagicItem* CastItem;
	RE::MagicSystem::CastingSource castingSource;
	RE::EnchantmentItem* ammoEnchantment;
	RE::AlchemyItem* poison;
	uint32_t area;
	float field_94;
	float scale;
	char field_9C;
	char field_9D;
	char field_9E;
	char field_9F;
	char field_A0;
	char field_A1;
	char field_A2;
	char field_A3;
	char gapA4[4];
};
static_assert(sizeof(Projectile__LaunchData) == 0xA8);

void init_Launchdata(Projectile__LaunchData& ldata, RE::Actor* a, const RE::NiPoint3& startPos, RE::SpellItem* spel,
	float rotationZ, float rotationX, uint32_t area)
{
	auto mgef = FenixUtils::getAVEffectSetting(spel);

	ldata.vftable_LaunchData_0 = nullptr;  // TODO: mb used
	ldata.startPos = startPos;
	ldata.Point_14 = { 0.0f, 0.0f, 0.0f };
	ldata.projectile = mgef->data.projectileBase;
	ldata.source = a;
	ldata.combatController = a->combatController;
	ldata.weap = nullptr;
	ldata.overwriteAmmo = nullptr;
	ldata.rotationZ = rotationZ;
	ldata.rotationX = rotationX;
	ldata.field_50 = nullptr;
	ldata.target = nullptr;
	ldata.drawn_time.first = 0.0f;
	ldata.drawn_time.second = 0.0f;
	ldata.cell = a->GetParentCell();
	ldata.CastItem = spel;
	ldata.castingSource = RE::MagicSystem::CastingSource::kOther;
	ldata.ammoEnchantment = nullptr;
	ldata.poison = nullptr;
	ldata.area = area;
	ldata.field_94 = 1.0f;
	ldata.scale = 1.0f;
	*(uint32_t*)&ldata.field_9C = 0x10000;
	ldata.field_A0 = 0;
	ldata.field_A1 = 0;
	ldata.field_A2 = 0;
}

uint32_t* CreateProjectile_14074B170(uint32_t* a1, Projectile__LaunchData* LaunchData)
{
	return _generic_foo_<42928, decltype(CreateProjectile_14074B170)>::eval(a1, LaunchData);
}

RE::Effect* GetCostiestEffect(RE::MagicItem* spel, RE::MagicSystem::Delivery type, char ignorearea)
{
	return _generic_foo_<11216, decltype(GetCostiestEffect)>::eval(spel, type, ignorearea);
}

uint32_t cast(RE::Actor* caster, RE::SpellItem* spel, const RE::NiPoint3& start_pos, const ProjectileRot& rot)
{
	Projectile__LaunchData ldata;
	auto eff = GetCostiestEffect(spel, RE::MagicSystem::Delivery::kAimed, false);

	init_Launchdata(ldata, caster, start_pos, spel, rot.z, rot.x, eff ? eff->GetArea() : 0);

	uint32_t handle;
	CreateProjectile_14074B170(&handle, &ldata);

	return handle;
}

uint32_t cast_CustomPos(RE::Actor* caster, RE::SpellItem* spel, const RE::NiPoint3& start_pos, const ProjectileRot& rot)
{
	auto handle = cast(caster, spel, start_pos, rot);

	RE::TESObjectREFRPtr _refr;
	RE::LookupReferenceByHandle(handle, _refr);
	if (auto proj = _refr.get() ? _refr.get()->As<RE::Projectile>() : nullptr) {
		set_CustomPosType(proj);
	}

	return handle;
}

namespace Sounds
{

	RE::BGSSoundDescriptorForm* EffectSetting__get_sndr(RE::EffectSetting* a1, RE::MagicSystem::SoundID sid)
	{
		return _generic_foo_<11001, decltype(EffectSetting__get_sndr)>::eval(a1, sid);
	}

	void PlaySound_func3_140BEDB10(RE::BSSoundHandle* a1, RE::NiAVObject* source_node)
	{
		return _generic_foo_<66375, decltype(PlaySound_func3_140BEDB10)>::eval(a1, source_node);
	}

	char set_sound_position(RE::BSSoundHandle* shandle, float x, float y, float z)
	{
		return _generic_foo_<66370, decltype(set_sound_position)>::eval(shandle, x, y, z);
	}

	void play_cast_sound(RE::Actor* caster, RE::SpellItem* spel, const RE::NiPoint3& start_pos)
	{
		if (auto root = caster->Get3D2()) {
			RE::BSSoundHandle shandle;
			RE::BGSSoundDescriptorForm* sndr = nullptr;
			auto sid = RE::MagicSystem::SoundID::kRelease;
			if (auto eff = FenixUtils::getAVEffectSetting(spel)) {
				sndr = EffectSetting__get_sndr(eff, sid);
			}
			RE::BSAudioManager::GetSingleton()->BuildSoundDataFromDescriptor(shandle, sndr, 0);
			PlaySound_func3_140BEDB10(&shandle, root);

			set_sound_position(&shandle, start_pos.x, start_pos.y, start_pos.z);
			shandle.Play();
		}
	}
}

uint32_t cast_CustomPos_withsound(RE::Actor* caster, RE::SpellItem* spel, const RE::NiPoint3& start_pos, const ProjectileRot& rot)
{
	auto handle = cast_CustomPos(caster, spel, start_pos, rot);

	Sounds::play_cast_sound(caster, spel, start_pos);

	return handle;
}

extern "C" 
{
	DLLEXPORT uint32_t FenixProjectilesAPI_cast(RE::Actor* caster, RE::SpellItem* spel, const RE::NiPoint3& start_pos,
		const ProjectileRot& rot)
	{
		return cast(caster, spel, start_pos, rot);
	}

	DLLEXPORT uint32_t FenixProjectilesAPI_cast_CustomPos(RE::Actor* caster, RE::SpellItem* spel, const RE::NiPoint3& start_pos,
		const ProjectileRot& rot)
	{
		return cast_CustomPos(caster, spel, start_pos, rot);
	}

	DLLEXPORT uint32_t FenixProjectilesAPI_cast_CustomPos_withsound(RE::Actor* caster, RE::SpellItem* spel,
		const RE::NiPoint3& start_pos,
		const ProjectileRot& rot)
	{
		return cast_CustomPos_withsound(caster, spel, start_pos, rot);
	}
}
