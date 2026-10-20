
set(LIBRARY_NAME scripts)

set (SCRIPTS_SRCS 

ScriptLoader.cpp
ScriptLoader.h
scriptPCH.h

battlegrounds/battleground.cpp
battlegrounds/battleground_alterac.cpp
battlegrounds/battleground_sunnyglade.cpp
dungeons/black_morass/black_morass.h
dungeons/black_morass/black_morass_trash.cpp
dungeons/black_morass/black_morass_trash.hpp
dungeons/black_morass/boss_chronormu.cpp
dungeons/black_morass/boss_chronormu.hpp
dungeons/black_morass/boss_gerastrasz.cpp
dungeons/black_morass/boss_gerastrasz.hpp
dungeons/black_morass/instance_black_morass.cpp
dungeons/blackfathom_deeps/blackfathom_deeps.h
dungeons/blackfathom_deeps/boss_velthelaxx_the_defiler.cpp
dungeons/blackfathom_deeps/instance_blackfathom_deeps.cpp
dungeons/blackrock_depths/arena_challenge_ai.cpp
dungeons/blackrock_depths/blackrock_depths.cpp
dungeons/blackrock_depths/blackrock_depths.h
dungeons/blackrock_depths/boss_anubshiah.cpp
dungeons/blackrock_depths/boss_emperor_dagran_thaurissan.cpp
dungeons/blackrock_depths/boss_general_angerforge.cpp
dungeons/blackrock_depths/boss_gorosh_the_dervish.cpp
dungeons/blackrock_depths/boss_grizzle.cpp
dungeons/blackrock_depths/boss_high_interrogator_gerstahn.cpp
dungeons/blackrock_depths/boss_magmus.cpp
dungeons/blackrock_depths/boss_tomb_of_seven.cpp
dungeons/blackrock_depths/instance_blackrock_depths.cpp
dungeons/blackrock_spire/blackrock_spire.h
dungeons/blackrock_spire/boss_halycon.cpp
dungeons/blackrock_spire/boss_highlord_omokk.cpp
dungeons/blackrock_spire/boss_mother_smolderweb.cpp
dungeons/blackrock_spire/boss_overlord_wyrmthalak.cpp
dungeons/blackrock_spire/boss_pyroguard_emberseer.cpp
dungeons/blackrock_spire/boss_quartermaster_zigris.cpp
dungeons/blackrock_spire/boss_shadow_hunter_voshgajin.cpp
dungeons/blackrock_spire/boss_the_beast.cpp
dungeons/blackrock_spire/boss_urok.cpp
dungeons/blackrock_spire/boss_warmaster_voone.cpp
dungeons/blackrock_spire/instance_blackrock_spire.cpp
dungeons/blackrock_spire/ubrs_trash.cpp
dungeons/blackwing_lair/blackwing_lair.h
dungeons/blackwing_lair/boss_broodlord_lashlayer.cpp
dungeons/blackwing_lair/boss_chromaggus.cpp
dungeons/blackwing_lair/boss_ebonroc.cpp
dungeons/blackwing_lair/boss_firemaw.cpp
dungeons/blackwing_lair/boss_flamegor.cpp
dungeons/blackwing_lair/boss_nefarian.cpp
dungeons/blackwing_lair/boss_razorgore.cpp
dungeons/blackwing_lair/boss_vaelastrasz.cpp
dungeons/blackwing_lair/boss_victor_nefarius.cpp
dungeons/blackwing_lair/instance_blackwing_lair.cpp
dungeons/deadmines/boss_mr_smite.cpp
dungeons/deadmines/deadmines.cpp
dungeons/deadmines/deadmines.h
dungeons/deadmines/instance_deadmines.cpp
dungeons/dragonmaw_retreat/boss_bogpaw_truthsay.cpp
dungeons/dragonmaw_retreat/boss_gowlfang.cpp
dungeons/dragonmaw_retreat/boss_halgan_redbrand.cpp
dungeons/dragonmaw_retreat/boss_searistrasz.cpp
dungeons/dragonmaw_retreat/boss_zuluhed_the_whacked.cpp
dungeons/dragonmaw_retreat/dragonmaw_retreat.h
dungeons/dragonmaw_retreat/instance_dragonmaw_retreat.cpp
dungeons/frostmane_hollow/boss_hailar_the_frigid.cpp
dungeons/frostmane_hollow/frostmane_hollow.h
dungeons/windhorn_canyon/boss_bonespeaker_narlgom.cpp
dungeons/windhorn_canyon/windhorn_canyon.cpp
dungeons/windhorn_canyon/windhorn_canyon.h
dungeons/stormwrought_ruins/boss_chieftain_stormsong.cpp
dungeons/stormwrought_ruins/boss_dagar_the_glutton.cpp
dungeons/stormwrought_ruins/boss_deathlord_tidebane.cpp
dungeons/stormwrought_ruins/boss_duke_balor_iv.cpp
dungeons/stormwrought_ruins/boss_eldermaw_the_primordial.cpp
dungeons/stormwrought_ruins/boss_ighalfor.cpp
dungeons/stormwrought_ruins/boss_lady_drazare.cpp
dungeons/stormwrought_ruins/boss_librarian_theodorus.cpp
dungeons/stormwrought_ruins/boss_mycellakos.cpp
dungeons/stormwrought_ruins/boss_oronok_torn_heart.cpp
dungeons/stormwrought_ruins/boss_subjugator_halthas_shadecrest.cpp
dungeons/stormwrought_ruins/instance_stormwrought_ruins.cpp
dungeons/stormwrought_ruins/stormwrought_ruins.h
dungeons/dire_maul/boss_gordok_king.cpp
dungeons/dire_maul/boss_immol_thar.cpp
dungeons/dire_maul/boss_tendris_warpwood.cpp
dungeons/dire_maul/boss_zevrim.cpp
dungeons/dire_maul/dire_maul.h
dungeons/dire_maul/dreadsteed_ritual.cpp
dungeons/dire_maul/instance_dire_maul.cpp
dungeons/dire_maul/npc_ecorcefer.cpp
dungeons/dire_maul/npc_pusillin.cpp
dungeons/emerald_sanctum/emerald_sanctum.h
dungeons/emerald_sanctum/instance_emerald_sanctum.cpp
dungeons/emerald_sanctum/boss_solnius.cpp
dungeons/gnomeregan/boss_thermaplugg.cpp
dungeons/gnomeregan/gnomeregan.cpp
dungeons/gnomeregan/gnomeregan.h
dungeons/gnomeregan/instance_gnomeregan.cpp
dungeons/hateforge_quarry/boss_bargul_blackhammer.cpp
dungeons/hateforge_quarry/boss_engineer_figgles.cpp
dungeons/hateforge_quarry/boss_hargesh_doomcaller.cpp
dungeons/hateforge_quarry/boss_hatereaver_annhilator.cpp
dungeons/hateforge_quarry/hateforge_quarry_trash.cpp
dungeons/karazhan_crypt/instance_karazhan_crypt.cpp
dungeons/lower_karazhan_halls/boss_brood_queen_araxxna.cpp
dungeons/lower_karazhan_halls/boss_blackwald_ii.cpp
dungeons/lower_karazhan_halls/boss_clawlord_howlfang.cpp
dungeons/lower_karazhan_halls/boss_grizikil.cpp
dungeons/lower_karazhan_halls/boss_moroes.cpp
dungeons/lower_karazhan_halls/instance_lower_karazhan_halls.cpp
dungeons/lower_karazhan_halls/lower_karazhan_halls.h
dungeons/upper_karazhan_halls/boss_anomalus.cpp
dungeons/upper_karazhan_halls/boss_echo_of_medivh.cpp
dungeons/upper_karazhan_halls/boss_incantagos.cpp
dungeons/upper_karazhan_halls/boss_keeper_gnarlmoon.cpp
dungeons/upper_karazhan_halls/boss_kings_council.cpp
dungeons/upper_karazhan_halls/boss_kruul.cpp
dungeons/upper_karazhan_halls/boss_sanv_tasdal.cpp
dungeons/crescent_grove/instance_crescent_grove.cpp
dungeons/gilneas_city/instance_gilneas_city.h
dungeons/gilneas_city/instance_gilneas_city.cpp
dungeons/gilneas_city/boss_celia.cpp
dungeons/gilneas_city/boss_lord_mortimer.cpp
dungeons/maraudon/boss_celebras_the_cursed.cpp
dungeons/maraudon/boss_landslide.cpp
dungeons/maraudon/boss_noxxion.cpp
dungeons/maraudon/boss_princess_theradras.cpp
dungeons/maraudon/instance_maraudon.cpp
dungeons/maraudon/maraudon.h
dungeons/molten_core/boss_baron_geddon.cpp
dungeons/molten_core/boss_garr.cpp
dungeons/molten_core/boss_gehennas.cpp
dungeons/molten_core/boss_golemagg.cpp
dungeons/molten_core/boss_incindis.cpp
dungeons/molten_core/boss_lucifron.cpp
dungeons/molten_core/boss_magmadar.cpp
dungeons/molten_core/boss_majordomo_executus.cpp
dungeons/molten_core/boss_ragnaros.cpp
dungeons/molten_core/boss_shazzrah.cpp
dungeons/molten_core/boss_sulfuron_harbinger.cpp
dungeons/molten_core/boss_thane.cpp
dungeons/molten_core/boss_twin_golems.cpp
dungeons/molten_core/instance_molten_core.cpp
dungeons/molten_core/molten_core.cpp
dungeons/molten_core/molten_core.h
dungeons/naxxramas/boss_anubrekhan.cpp
dungeons/naxxramas/boss_faerlina.cpp
dungeons/naxxramas/boss_four_horsemen.cpp
dungeons/naxxramas/boss_gluth.cpp
dungeons/naxxramas/boss_gothik.cpp
dungeons/naxxramas/boss_grobbulus.cpp
dungeons/naxxramas/boss_heigan.cpp
dungeons/naxxramas/boss_kelthuzad.cpp
dungeons/naxxramas/boss_loatheb.cpp
dungeons/naxxramas/boss_maexxna.cpp
dungeons/naxxramas/boss_noth.cpp
dungeons/naxxramas/boss_patchwerk.cpp
dungeons/naxxramas/boss_razuvious.cpp
dungeons/naxxramas/boss_sapphiron.cpp
dungeons/naxxramas/boss_thaddius.cpp
dungeons/naxxramas/instance_naxxramas.cpp
dungeons/naxxramas/naxxramas.h
dungeons/onyxias_lair/boss_onyxia.cpp
dungeons/onyxias_lair/instance_onyxia_lair.cpp
dungeons/onyxias_lair/instance_onyxia_lair.h
dungeons/razorfen_downs/instance_razorfen_downs.cpp
dungeons/razorfen_downs/razorfen_downs.cpp
dungeons/razorfen_downs/razorfen_downs.h
dungeons/razorfen_kraul/instance_razorfen_kraul.cpp
dungeons/razorfen_kraul/razorfen_kraul.cpp
dungeons/razorfen_kraul/razorfen_kraul.h
dungeons/ruins_of_ahnqiraj/boss_ayamiss.cpp
dungeons/ruins_of_ahnqiraj/boss_buru.cpp
dungeons/ruins_of_ahnqiraj/boss_kurinnaxx.cpp
dungeons/ruins_of_ahnqiraj/boss_moam.cpp
dungeons/ruins_of_ahnqiraj/boss_ossirian.cpp
dungeons/ruins_of_ahnqiraj/instance_ruins_of_ahnqiraj.cpp
dungeons/ruins_of_ahnqiraj/npc_sandstalker.cpp
dungeons/ruins_of_ahnqiraj/ruins_of_ahnqiraj.cpp
dungeons/ruins_of_ahnqiraj/ruins_of_ahnqiraj.h
dungeons/scarlet_citadel/boss_abbendis.cpp
dungeons/scarlet_citadel/boss_abbendis.hpp
dungeons/scarlet_citadel/boss_ardaeus.cpp
dungeons/scarlet_citadel/boss_ardaeus.hpp
dungeons/scarlet_citadel/boss_daelus.cpp
dungeons/scarlet_citadel/boss_daelus.hpp
dungeons/scarlet_citadel/boss_mariella.cpp
dungeons/scarlet_citadel/boss_mariella.hpp
dungeons/scarlet_citadel/instance_scarlet_citadel.cpp
dungeons/scarlet_citadel/trashbosses_scarlet_citadel.cpp
dungeons/scarlet_citadel/trashbosses_scarlet_citadel.hpp
dungeons/scarlet_citadel/trashmobs_scarlet_citadel.cpp
dungeons/scarlet_citadel/trashmobs_scarlet_citadel.hpp
dungeons/scarlet_citadel/scarlet_citadel.h
dungeons/scarlet_monastery/boss_arcanist_doan.cpp
dungeons/scarlet_monastery/boss_azshir_the_sleepless.cpp
dungeons/scarlet_monastery/boss_bloodmage_thalnos.cpp
dungeons/scarlet_monastery/boss_headless_horseman.cpp
dungeons/scarlet_monastery/boss_herod.cpp
dungeons/scarlet_monastery/boss_high_inquisitor_fairbanks.cpp
dungeons/scarlet_monastery/boss_houndmaster_loksey.cpp
dungeons/scarlet_monastery/boss_interrogator_vishas.cpp
dungeons/scarlet_monastery/boss_scorn.cpp
dungeons/scarlet_monastery/instance_scarlet_monastery.cpp
dungeons/scarlet_monastery/scarlet_monastery.h
dungeons/scholomance/boss_darkmaster_gandling.cpp
dungeons/scholomance/boss_doctor_theolen_krastinov.cpp
dungeons/scholomance/boss_illucia_barov.cpp
dungeons/scholomance/boss_instructor_malicia.cpp
dungeons/scholomance/boss_jandice_barov.cpp
dungeons/scholomance/boss_kormok.cpp
dungeons/scholomance/boss_lord_alexei_barov.cpp
dungeons/scholomance/boss_lorekeeper_polkelt.cpp
dungeons/scholomance/boss_ras_frostwhisper.cpp
dungeons/scholomance/boss_the_ravenian.cpp
dungeons/scholomance/boss_vectus.cpp
dungeons/scholomance/instance_scholomance.cpp
dungeons/scholomance/scholo_trash.cpp
dungeons/scholomance/scholomance.h
dungeons/shadowfang_keep/instance_shadowfang_keep.cpp
dungeons/stormwind_vaults/boss_aszosh_grimflame.cpp
dungeons/stormwind_vaults/boss_black_bride.cpp
dungeons/stormwind_vaults/boss_damian_the_ripper.cpp
dungeons/stormwind_vaults/boss_arctiras.cpp
dungeons/stormwind_vaults/boss_nazorna.cpp
dungeons/stormwind_vaults/boss_thamgrarr.cpp
dungeons/stormwind_vaults/boss_volkan_cruelblade.cpp
dungeons/stormwind_vaults/instance_stormwind_vaults.cpp
dungeons/stratholme/boss_atiesh.cpp
dungeons/stratholme/boss_baroness_anastari.cpp
dungeons/stratholme/boss_cannon_master_willey.cpp
dungeons/stratholme/boss_dathrohan_balnazzar.cpp
dungeons/stratholme/boss_magistrate_barthilas.cpp
dungeons/stratholme/boss_maleki_the_pallid.cpp
dungeons/stratholme/boss_nerubenkan.cpp
dungeons/stratholme/boss_order_of_silver_hand.cpp
dungeons/stratholme/boss_postmaster_malown.cpp
dungeons/stratholme/boss_ramstein_the_gorger.cpp
dungeons/stratholme/boss_sc_attunement.cpp
dungeons/stratholme/boss_timmy_the_cruel.cpp
dungeons/stratholme/instance_stratholme.cpp
dungeons/stratholme/stratholme.cpp
dungeons/stratholme/stratholme.h
dungeons/sunken_temple/instance_sunken_temple.cpp
dungeons/sunken_temple/sunken_temple.cpp
dungeons/sunken_temple/sunken_temple.h
dungeons/temple_of_ahnqiraj/boss_bug_trio.cpp
dungeons/temple_of_ahnqiraj/boss_cthun.cpp
dungeons/temple_of_ahnqiraj/boss_fankriss.cpp
dungeons/temple_of_ahnqiraj/boss_huhuran.cpp
dungeons/temple_of_ahnqiraj/boss_ouro.cpp
dungeons/temple_of_ahnqiraj/boss_sartura.cpp
dungeons/temple_of_ahnqiraj/boss_skeram.cpp
dungeons/temple_of_ahnqiraj/boss_twinemperors.cpp
dungeons/temple_of_ahnqiraj/boss_viscidus.cpp
dungeons/temple_of_ahnqiraj/instance_temple_of_ahnqiraj.cpp
dungeons/temple_of_ahnqiraj/mob_anubisath_sentinel.cpp
dungeons/temple_of_ahnqiraj/temple_of_ahnqiraj.h
dungeons/uldaman/boss_archaedas.cpp
dungeons/uldaman/boss_ironaya.cpp
dungeons/uldaman/instance_uldaman.cpp
dungeons/uldaman/uldaman.cpp
dungeons/uldaman/uldaman.h
dungeons/wailing_caverns/def_wailing_caverns.h
dungeons/wailing_caverns/instance_wailing_caverns.cpp
dungeons/wailing_caverns/wailing_caverns.cpp
dungeons/zulfarrak/instance_zulfarrak.cpp
dungeons/zulfarrak/farraki_arena.cpp
dungeons/zulfarrak/zulfarrak.cpp
dungeons/zulfarrak/zulfarrak.h
dungeons/zulgurub/boss_arlokk.cpp
dungeons/zulgurub/boss_gahzranka.cpp
dungeons/zulgurub/boss_grilek.cpp
dungeons/zulgurub/boss_hakkar.cpp
dungeons/zulgurub/boss_hazzarah.cpp
dungeons/zulgurub/boss_jeklik.cpp
dungeons/zulgurub/boss_jindo.cpp
dungeons/zulgurub/boss_mandokir.cpp
dungeons/zulgurub/boss_marli.cpp
dungeons/zulgurub/boss_renataki.cpp
dungeons/zulgurub/boss_thekal.cpp
dungeons/zulgurub/boss_venoxis.cpp
dungeons/zulgurub/boss_wushoolay.cpp
dungeons/zulgurub/instance_zulgurub.cpp
dungeons/zulgurub/zulgurub.h
dungeons/zulgurub/zulgurub_trash.cpp
events/boss_dark_reaver.cpp
events/boss_emeriss.cpp
events/boss_lethon.cpp
events/boss_lord_kazzak.cpp
events/boss_nerubian_overseer.cpp
events/boss_ostarius.cpp
events/boss_pompa.cpp
events/boss_rares.cpp
events/boss_snowball.cpp
events/boss_taerar.cpp
events/boss_turtlhu.cpp
events/boss_ysondre.cpp
events/event_bloody_coins.cpp
events/event_dragons_of_nightmare.cpp
events/event_dragons_of_nightmare.h
events/event_elemental_invasions.cpp
events/event_fireworks_show.cpp
events/event_mirage_raceway.cpp
events/event_naxxramas.cpp
events/event_naxxramas.h
events/event_scourge_invasion.cpp
events/event_wareffort.cpp
events/event_wareffort.h
events/event_dreadsteed.h
events/quest_stormwind_rendezvous.cpp
events/quest_stormwind_rendezvous.h

miscellaneous/areatrigger_scripts.cpp
miscellaneous/gameobject_scripts.cpp
miscellaneous/feature_custom_spell_test.cpp
miscellaneous/feature_gardening.cpp
miscellaneous/feature_transmog.h
miscellaneous/item_orb_of_draconic_energy.cpp
miscellaneous/npc_loothelper.h
miscellaneous/npc_loothelper.cpp
miscellaneous/ptr.cpp
miscellaneous/random_scripts_0.cpp
miscellaneous/random_scripts_1.cpp
miscellaneous/random_scripts_2.cpp
miscellaneous/random_scripts_3.cpp
miscellaneous/custom_exploration_triggers.cpp
miscellaneous/jewelcrafting.cpp

spells/spell_druid.cpp
spells/spell_hunter.cpp
spells/spell_item.cpp
spells/spell_mage.cpp
spells/spell_paladin.cpp
spells/spell_priest.cpp
spells/spell_rogue.cpp
spells/spell_shaman.cpp
spells/spell_special.cpp
spells/spells_turtle.cpp
spells/spell_warlock.cpp
spells/spell_warrior.cpp

world/alterac_mountains.cpp
world/arathi_highlands.cpp
world/ashenvale.cpp
world/alahthalas.cpp
world/azshara.cpp
world/balor.cpp
world/blasted_lands.cpp
world/boss_omen.cpp
world/boss_omen.h
world/burning_steppes.cpp
world/darkshore.cpp
world/desolace.cpp
world/dun_morogh.cpp
world/durotar.cpp
world/duskwood.cpp
world/dustwallow_marsh.cpp
world/eastern_plaguelands.cpp
world/elwynn_forest.cpp
world/felwood.cpp
world/feralas.cpp
world/grim_reaches.cpp
world/hillsbrad_foothills.cpp
world/hinterlands.cpp
world/ironforge.cpp
world/loch_modan.cpp
world/moonglade.cpp
world/moonwhisper_coast.cpp
world/mulgore.cpp
world/northwind.cpp
world/orgrimmar.cpp
world/redridge_mountains.cpp
world/searing_gorge.cpp
world/silithus.cpp
world/silithus/field_duty_alliance/defines.h
world/silithus/field_duty_alliance/npc_arcanist_nozzlespring.cpp
world/silithus/field_duty_alliance/npc_arcanist_nozzlespring.h
world/silithus/field_duty_alliance/npc_captain_blackanvil.cpp
world/silithus/field_duty_alliance/npc_captain_blackanvil.h
world/silithus/field_duty_alliance/npc_hivezora_abomination.cpp
world/silithus/field_duty_alliance/npc_hivezora_abomination.h
world/silithus/field_duty_alliance/npc_janela_stouthammer.cpp
world/silithus/field_duty_alliance/npc_janela_stouthammer.h
world/silithus/field_duty_alliance/trigger_field_duty_alliance.cpp
world/silithus/field_duty_alliance/trigger_field_duty_alliance.h
world/silithus/field_duty_horde/defines.h
world/silithus/field_duty_horde/npc_hiveregal_hunterkiller.h
world/silithus/field_duty_horde/npc_krug_skullsplit.h
world/silithus/field_duty_horde/npc_merok_longstride.h
world/silithus/field_duty_horde/npc_orgrimmar_legion_grunt.h
world/silithus/field_duty_horde/npc_shadow_priestess_shai.h
world/silithus/silithus.h
world/silverpine_forest.cpp
world/stonetalon_mountains.cpp
world/stranglethorn_vale.cpp
world/swamp_of_sorrows.cpp
world/tanaris.cpp
world/teldrassil.cpp
world/the_barrens.cpp
world/thousand_needles.cpp
world/thunder_bluff.cpp
world/tirisfal_glades.cpp
world/undercity.cpp
world/ungoro_crater.cpp
world/western_plaguelands.cpp
world/westfall.cpp
world/wetlands.cpp
world/winterspring.cpp
world/stormwind_city.cpp
)

if((${CMAKE_VERSION} VERSION_LESS "3.16") OR USE_PCH_OLD)
  list(APPEND SCRIPTS_SRCS
        scriptPCH.cpp
    )
  # Enable precompiled headers when using the GCC compiler.
  if(USE_PCH)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
  endif()
endif()

if(WIN32)
  if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  endif()
endif()

# Used in game ScriptMgr, not need here
list(REMOVE_ITEM SCRIPTS_SRCS ScriptLoader.cpp)
list(REMOVE_ITEM SCRIPTS_SRCS ScriptLoader.h)

foreach(SRC ${SCRIPTS_SRCS})
  get_filename_component(PTH ${SRC} PATH)
  if(PTH)
    if(NOT XCODE) # FIXME: Xcode Generator has bug with nested dirs
      string(REPLACE "/" "\\\\" PTH ${PTH})
    endif()
    source_group(${PTH} FILES ${SRC})
  endif()
endforeach(SRC)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src/shared
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/framework/Network
  ${CMAKE_SOURCE_DIR}/src/game
  ${CMAKE_SOURCE_DIR}/src/game/AI
  ${CMAKE_SOURCE_DIR}/src/game/Anticheat
  ${CMAKE_SOURCE_DIR}/src/game/AuctionHouse
  ${CMAKE_SOURCE_DIR}/src/game/Battlegrounds
  ${CMAKE_SOURCE_DIR}/src/game/Chat
  ${CMAKE_SOURCE_DIR}/src/game/Commands
  ${CMAKE_SOURCE_DIR}/src/game/Database
  ${CMAKE_SOURCE_DIR}/src/game/Group
  ${CMAKE_SOURCE_DIR}/src/game/Guild
  ${CMAKE_SOURCE_DIR}/src/game/Handlers
  ${CMAKE_SOURCE_DIR}/src/game/LFG
  ${CMAKE_SOURCE_DIR}/src/game/Mail
  ${CMAKE_SOURCE_DIR}/src/game/Maps
  ${CMAKE_SOURCE_DIR}/src/game/Maps/Pool
  ${CMAKE_SOURCE_DIR}/src/game/Movement
  ${CMAKE_SOURCE_DIR}/src/game/Movement/spline
  ${CMAKE_SOURCE_DIR}/src/game/Objects
  ${CMAKE_SOURCE_DIR}/src/game/OutdoorPvP
  ${CMAKE_SOURCE_DIR}/src/game/Protocol
  ${CMAKE_SOURCE_DIR}/src/game/Spells
  ${CMAKE_SOURCE_DIR}/src/game/Threat
  ${CMAKE_SOURCE_DIR}/src/game/Transports
  ${CMAKE_SOURCE_DIR}/src/game/vmap
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation
  ${CMAKE_SOURCE_DIR}/dep/include/g3dlite
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/src/shared
  ${TBB_INCLUDE_DIRS}
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
)

add_library(${LIBRARY_NAME} STATIC
  ${SCRIPTS_SRCS}
)

# MSVC's /GL intermediate representation makes this exceptionally large
# generated script archive exceed the COFF librarian's 4 GiB hard limit.
# Keep LTCG on the core, bot module and final executable; ordinary /O2 is the
# only viable release mode for this archive.
if(MSVC AND (ENABLE_LTO OR PROFILE_GENERATE OR PROFILE_USE))
  target_compile_options(${LIBRARY_NAME} PRIVATE $<$<CONFIG:Release>:/GL->)
endif()

SET_TARGET_PROPERTIES (${LIBRARY_NAME} PROPERTIES FOLDER "Game Libs")

target_link_libraries(scripts
  game
)

if(WIN32)
	if (ENABLE_PROFILING)
		include_directories(
			${OPTICK_INCLUDE_DIR}
    )
	endif()
endif()

#add_dependencies(${LIBRARY_NAME} revision.h)

if((${CMAKE_VERSION} VERSION_LESS "3.16") OR USE_PCH_OLD)
  if(USE_PCH)
    if(MSVC OR XCODE)
      if(MSVC)
        set(scripts_pch "${CMAKE_CURRENT_SOURCE_DIR}/scriptPCH.cpp")
      endif()
      add_native_precompiled_header(scripts ${CMAKE_CURRENT_SOURCE_DIR}/scriptPCH.h)
    elseif(CMAKE_COMPILER_IS_GNUCXX)
      add_precompiled_header(scripts ${CMAKE_CURRENT_SOURCE_DIR}/scriptPCH.h)
    endif()
  endif()
else()
  set(PCHS
      ../game/Mail/Mail.h
      ../game/Commands/Nostalrius.h
      ../game/ScriptMgr.h
      ../game/Objects/Object.h
      ../game/ObjectMgr.h
      ../game/Objects/Unit.h
      ../game/Spells/Spell.h
      ../game/Objects/Creature.h
      ../game/AI/CreatureAI.h
      ../game/Objects/GameObject.h
      ../game/GameEventMgr.h
      ../game/AI/ScriptedAI.h
      ../game/AI/ScriptedPetAI.h
      ../game/AI/ScriptedGameObject.h
      ../game/AI/ScriptedFollowerAI.h
      ../game/AI/ScriptedEscortAI.h
      ../game/ScriptedGossip.h
      ../game/Maps/GridSearchers.h
      ../game/AI/ScriptedInstance.h
      ../game/Spells/SpellAuras.h
      ../game/Objects/TemporarySummon.h
      ../game/AI/NullCreatureAI.h
      ../game/Chat/Chat.h
      ../game/Battlegrounds/BattleGroundAV.h
      ../game/World.h
      ../game/Weather.h
      ../game/AI/TotemAI.h
      scriptPCH.h
      )

  if(WIN32)
      list(APPEND PCHS <windows.h>)
  endif()

  # Generate precompiled header
  if(USE_PCH)
      target_precompile_headers(${LIBRARY_NAME} PRIVATE
          ${PCHS}
          )
  endif()
endif()

if(NOT USE_PCH)
  if(MSVC)
    target_compile_options(${LIBRARY_NAME} PRIVATE "/FI${CMAKE_CURRENT_SOURCE_DIR}/scriptPCH.h")
  else()
    target_compile_options(${LIBRARY_NAME} PRIVATE "-include${CMAKE_CURRENT_SOURCE_DIR}/scriptPCH.h")
  endif()
endif()
