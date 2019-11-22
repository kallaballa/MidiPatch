// Local configuration of miby MIDI event handlers
// Note that this requires the fn's being used to be 
// already declared elsewhere ..

#ifndef MIBYCONFIG_H
#define MIBYCONFIG_H

#warning "Using midipatch_user_config for miby"

// miby configuration macro's are set up by miby.h first
// here we un-def the ones we're going to override in
// order to avoid #warning overload

#ifdef MIBY_SYSEX_BUF_LEN
#warning "BUF ALREADY DEFINED"
#undef MIBY_SYSEX_BUF_LEN
#endif
#define MIBY_SYSEX_BUF_LEN	( 32 )

#ifdef MIBY_HND_CTRL_CHG
#undef MIBY_HND_CTRL_CHG
#endif
extern void MIDIPATCH_Control_Change(miby_this_t);
#define MIBY_HND_CTRL_CHG  MIDIPATCH_Control_Change

#ifdef MIBY_HND_NOTE_ON
#undef MIBY_HND_NOTE_ON
#endif
extern void MIDIPATCH_Note_On(miby_this_t);
#define MIBY_HND_NOTE_ON  MIDIPATCH_Note_On

#ifdef MIBY_HND_NOTE_OFF
#undef MIBY_HND_NOTE_OFF
#endif
extern void MIDIPATCH_Note_Off(miby_this_t);
#define MIBY_HND_NOTE_OFF  MIDIPATCH_Note_Off

#ifdef MIBY_HND_RT_START
#undef MIBY_HND_RT_START
#endif
extern void MIDIPATCH_Start(miby_this_t);
#define MIBY_HND_RT_START  MIDIPATCH_Start

#ifdef MIBY_HND_RT_STOP
#undef MIBY_HND_RT_STOP
#endif
extern void MIDIPATCH_Stop(miby_this_t);
#define MIBY_HND_RT_STOP  MIDIPATCH_Stop

#ifdef MIBY_HND_PROG_CHG
#undef MIBY_HND_PROG_CHG
#endif
extern void MIDIPATCH_Program_Change(miby_this_t);
#define MIBY_HND_PROG_CHG  MIDIPATCH_Program_Change

#if 0
extern void test_rt_timing_clock(miby_this_t);
#define MIBY_HND_RT_CLOCK	test_rt_timing_clock

extern void test_rt_start(miby_this_t);
#define MIBY_HND_RT_START	test_rt_start

extern void test_rt_continue(miby_this_t);
#define MIBY_HND_RT_CONTINUE	test_rt_continue

extern void test_rt_stop(miby_this_t);
#define MIBY_HND_RT_STOP	test_rt_stop

extern void test_rt_active_sense(miby_this_t);
#define MIBY_HND_RT_ACT_SENSE	test_rt_active_sense

extern void test_rt_system_reset(miby_this_t);
#define MIBY_HND_RT_SYS_RESET	test_rt_system_reset

extern void test_tunereq(miby_this_t);
#define MIBY_HND_SYS_TIMEREQ	test_tunereq

extern void test_mtc(miby_this_t);
#define MIBY_HND_SYS_MTC	test_mtc

extern void test_songpos(miby_this_t);
#define MIBY_HND_SYS_SONGPOS	test_songpos

extern void test_songsel(miby_this_t);
#define MIBY_HND_SYS_SONGSEL	test_songsel

extern void test_sysex(miby_this_t);
#define MIBY_HND_SYS_EX		test_sysex

extern void test_note_on(miby_this_t);
#define MIBY_HND_NOTE_ON	test_note_on

extern void test_note_off(miby_this_t);
#define MIBY_HND_NOTE_OFF	test_note_off

extern void test_poly_at(miby_this_t);
#define MIBY_HND_POLY_AT	test_poly_at

extern void test_cc(miby_this_t);
#define MIBY_HND_CTRL_CHG	test_cc

extern void test_pb(miby_this_t);
#define MIBY_HND_PITCHBEND	test_pb

extern void test_pc(miby_this_t);
#define MIBY_HND_PROG_CHG	test_pc

extern void test_chanat(miby_this_t);
#define MIBY_HND_CHAN_AT	test_chanat

#endif


#endif							/* MIBYCONFIG_H */
