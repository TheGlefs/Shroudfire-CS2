"""Generate CS2 Workshop sound events for the supplied voice clips."""

from pathlib import Path


SOUNDS = (
    "face_the_invisant_fear_nothing",
    "the_invisant_wants_blood_spill_its_own",
    "the_invisant_thinks_its_safe_prove_it_wrong",
    "the_invisant_can_bleed_make_it",
    "the_invisant_needs_an_opening_give_it_none",
    "the_invisant_is_closing_in_stand_your_ground",
    "the_hunger_is_here_find_them",
    "time_is_running_out_strike",
    "your_strength_is_fading_hunt",
    "your_invisibility_is_failing_end_this",
    "the_veil_is_breaking_finish_the_hunt",
    "the_shroud_is_fading_kill_them_now",
    "your_advantage_is_fading_waste_no_time",
    "the_shroud_demands_blood_take_it",
    "you_are_starving_make_the_next_strike_count",
    "every_second_weakens_you_strike",
    "you_have_stayed_here_too_long_move",
    "stay_there_and_this_place_becomes_your_grave",
    "move_now_or_be_removed",
    "keep_moving_death_is_closing_in",
    "you_have_lingered_long_enough_go",
    "find_the_gap_take_the_kill",
    "they_cannot_see_you_make_them_afraid",
    "the_hunt_begins_leave_no_surviors",
    "get_close_strike_fast",
    "they_are_armed_they_are_afraid",
    "choose_one_leave_none",
    "armitive_win",
    "armitive_win2",
    "armitive_win3",
    "invisant_win",
    "invisant_win2",
    "invisant_win3",
    "round_draw",
    "round_draw2",
    "round_draw3",
    "wake_up_take_a_step",
    "breaks_over_come_back",
    "eyes_up_youre_still_in_the_game",
    "the_fight_continues_with_or_without_you",
    "still_there_not_for_much_longer",
    "first_blood",
    "double_kill",
    "killing_spree",
    "ultra_kill",
    "rampage",
    "dominating",
    "unstoppable",
    "monster_kill",
    "godlike",
    "holy_shit",
)

root = Path(__file__).resolve().parents[1]
addon = root / "sound-pack"
sounds = addon / "sounds" / "shroudfire"
events = addon / "soundevents" / "soundevents_addon.vsndevts"
movement_overrides = addon / "soundevents" / "movement_overrides.vsndevts.inc"
sounds.mkdir(parents=True, exist_ok=True)
events.parent.mkdir(parents=True, exist_ok=True)

for name in SOUNDS:
    source = sounds / f"{name}.mp3"
    if not source.is_file():
        raise FileNotFoundError(source)

unexpected = sorted(staged.name for staged in sounds.glob("*.mp3") if staged.stem not in SOUNDS)
if unexpected:
    raise SystemExit(f"MP3 files missing from SOUNDS: {', '.join(unexpected)}. Update the clip list first.")
if not movement_overrides.is_file():
    raise FileNotFoundError(movement_overrides)

lines = [
    "<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->",
    "{",
]
for name in SOUNDS:
    lines.extend((
        f'    "shroudfire.{name}" =',
        "    {",
        '        type = "csgo_mega"',
        "        volume = 1",
        "        pitch = 1",
        f'        vsnd_files_track_01 = "sounds/shroudfire/{name}.vsnd"',
        "    }",
    ))
for name in ("Event.RoundDraw", "Announcer.RoundDraw.CS2_Classic"):
    lines.extend((
        f'    "{name}" =',
        "    {",
        '        type = "csgo_mega"',
        "        volume = 0",
        "        pitch = 1",
        '        vsnd_files_track_01 = "sounds/vo/announcer/cs2_classic/rounddraw.vsnd"',
        "    }",
    ))
lines.extend(movement_overrides.read_text(encoding="utf-8").rstrip().splitlines())
lines.append("}")
events.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
print(f"Generated events for {len(SOUNDS)} sounds in {addon}")
