#pragma once

#include "..\..\all\enable_all.h"


namespace LSW {
	namespace v2 {
		namespace Events {

			enum hack_strings {
				HACK_NONE,
				HACK_IMMORTAL,
				HACK_NEXTLEVEL
			};
			enum events_keys {
				KEY_A = 1,
				KEY_B = 2,
				KEY_C = 3,
				KEY_D = 4,
				KEY_E = 5,
				KEY_F = 6,
				KEY_G = 7,
				KEY_H = 8,
				KEY_I = 9,
				KEY_J = 10,
				KEY_K = 11,
				KEY_L = 12,
				KEY_M = 13,
				KEY_N = 14,
				KEY_O = 15,
				KEY_P = 16,
				KEY_Q = 17,
				KEY_R = 18,
				KEY_S = 19,
				KEY_T = 20,
				KEY_U = 21,
				KEY_V = 22,
				KEY_W = 23,
				KEY_X = 24,
				KEY_Y = 25,
				KEY_Z = 26,

				KEY_0 = 27,
				KEY_1 = 28,
				KEY_2 = 29,
				KEY_3 = 30,
				KEY_4 = 31,
				KEY_5 = 32,
				KEY_6 = 33,
				KEY_7 = 34,
				KEY_8 = 35,
				KEY_9 = 36,

				KEY_PAD_0 = 37,
				KEY_PAD_1 = 38,
				KEY_PAD_2 = 39,
				KEY_PAD_3 = 40,
				KEY_PAD_4 = 41,
				KEY_PAD_5 = 42,
				KEY_PAD_6 = 43,
				KEY_PAD_7 = 44,
				KEY_PAD_8 = 45,
				KEY_PAD_9 = 46,

				KEY_F1 = 47,
				KEY_F2 = 48,
				KEY_F3 = 49,
				KEY_F4 = 50,
				KEY_F5 = 51,
				KEY_F6 = 52,
				KEY_F7 = 53,
				KEY_F8 = 54,
				KEY_F9 = 55,
				KEY_F10 = 56,
				KEY_F11 = 57,
				KEY_F12 = 58,

				KEY_ESCAPE = 59,
				KEY_TILDE = 60,
				KEY_MINUS = 61,
				KEY_EQUALS = 62,
				KEY_BACKSPACE = 63,
				KEY_TAB = 64,
				KEY_OPENBRACE = 65,
				KEY_CLOSEBRACE = 66,
				KEY_ENTER = 67,
				KEY_SEMICOLON = 68,
				KEY_QUOTE = 69,
				KEY_BACKSLASH = 70,
				KEY_BACKSLASH2 = 71,
				KEY_COMMA = 72,
				KEY_FULLSTOP = 73,
				KEY_SLASH = 74,
				KEY_SPACE = 75,

				KEY_INSERT = 76,
				KEY_DELETE = 77,
				KEY_HOME = 78,
				KEY_END = 79,
				KEY_PGUP = 80,
				KEY_PGDN = 81,
				KEY_LEFT = 82,
				KEY_RIGHT = 83,
				KEY_UP = 84,
				KEY_DOWN = 85,

				KEY_PAD_SLASH = 86,
				KEY_PAD_ASTERISK = 87,
				KEY_PAD_MINUS = 88,
				KEY_PAD_PLUS = 89,
				KEY_PAD_DELETE = 90,
				KEY_PAD_ENTER = 91,

				KEY_PRINTSCREEN = 92,
				KEY_PAUSE = 93,

				KEY_ABNT_C1 = 94,
				KEY_YEN = 95,
				KEY_KANA = 96,
				KEY_CONVERT = 97,
				KEY_NOCONVERT = 98,
				KEY_AT = 99,
				KEY_CIRCUMFLEX = 100,
				KEY_COLON2 = 101,
				KEY_KANJI = 102,

				KEY_PAD_EQUALS = 103,
				KEY_BACKQUOTE = 104,
				KEY_SEMICOLON2 = 105,
				KEY_COMMAND = 106,

				KEY_BACK = 107,
				KEY_VOLUME_UP = 108,
				KEY_VOLUME_DOWN = 109,

				// Android game keys
				KEY_SEARCH = 110,
				KEY_DPAD_CENTER = 111,
				KEY_BUTTON_X = 112,
				KEY_BUTTON_Y = 113,
				KEY_DPAD_UP = 114,
				KEY_DPAD_DOWN = 115,
				KEY_DPAD_LEFT = 116,
				KEY_DPAD_RIGHT = 117,
				KEY_SELECT = 118,
				KEY_START = 119,
				KEY_BUTTON_L1 = 120,
				KEY_BUTTON_R1 = 121,
				KEY_BUTTON_L2 = 122,
				KEY_BUTTON_R2 = 123,
				KEY_BUTTON_A = 124,
				KEY_BUTTON_B = 125,
				KEY_THUMBL = 126,
				KEY_THUMBR = 127,

				KEY_UNKNOWN = 128,

				KEY_UNKNOWN_0 = 129,
				KEY_UNKNOWN_1 = 130,
				KEY_UNKNOWN_2 = 131,
				KEY_UNKNOWN_3 = 132,
				KEY_UNKNOWN_4 = 133,
				KEY_UNKNOWN_5 = 134,
				KEY_UNKNOWN_6 = 135,
				KEY_UNKNOWN_7 = 136,
				KEY_UNKNOWN_8 = 137,
				KEY_UNKNOWN_9 = 138,
				KEY_UNKNOWN_10 = 139,
				KEY_UNKNOWN_11 = 140,
				KEY_UNKNOWN_12 = 141,
				KEY_UNKNOWN_13 = 142,
				KEY_UNKNOWN_14 = 143,
				KEY_UNKNOWN_15 = 144,
				KEY_UNKNOWN_16 = 145,
				KEY_UNKNOWN_17 = 146,
				KEY_UNKNOWN_18 = 147,
				KEY_UNKNOWN_19 = 148,
				KEY_UNKNOWN_20 = 149,
				KEY_UNKNOWN_21 = 150,
				KEY_UNKNOWN_22 = 151,
				KEY_UNKNOWN_23 = 152,
				KEY_UNKNOWN_24 = 153,
				KEY_UNKNOWN_25 = 154,
				KEY_UNKNOWN_26 = 155,
				KEY_UNKNOWN_27 = 156,
				KEY_UNKNOWN_28 = 157,
				KEY_UNKNOWN_29 = 158,
				KEY_UNKNOWN_30 = 159,
				KEY_UNKNOWN_31 = 160,
				KEY_UNKNOWN_32 = 161,
				KEY_UNKNOWN_33 = 162,
				KEY_UNKNOWN_34 = 163,
				KEY_UNKNOWN_35 = 164,
				KEY_UNKNOWN_36 = 165,
				KEY_UNKNOWN_37 = 166,
				KEY_UNKNOWN_38 = 167,
				KEY_UNKNOWN_39 = 168,
				KEY_UNKNOWN_40 = 169,
				KEY_UNKNOWN_41 = 170,
				KEY_UNKNOWN_42 = 171,
				KEY_UNKNOWN_43 = 172,
				KEY_UNKNOWN_44 = 173,
				KEY_UNKNOWN_45 = 174,
				KEY_UNKNOWN_46 = 175,
				KEY_UNKNOWN_47 = 176,
				KEY_UNKNOWN_48 = 177,
				KEY_UNKNOWN_49 = 178,
				KEY_UNKNOWN_50 = 179,
				KEY_UNKNOWN_51 = 180,
				KEY_UNKNOWN_52 = 181,
				KEY_UNKNOWN_53 = 182,
				KEY_UNKNOWN_54 = 183,
				KEY_UNKNOWN_55 = 184,
				KEY_UNKNOWN_56 = 185,
				KEY_UNKNOWN_57 = 186,
				KEY_UNKNOWN_58 = 187,
				KEY_UNKNOWN_59 = 188,
				KEY_UNKNOWN_60 = 189,
				KEY_UNKNOWN_61 = 190,
				KEY_UNKNOWN_62 = 191,
				KEY_UNKNOWN_63 = 192,
				KEY_UNKNOWN_64 = 193,
				KEY_UNKNOWN_65 = 194,
				KEY_UNKNOWN_66 = 195,
				KEY_UNKNOWN_67 = 196,
				KEY_UNKNOWN_68 = 197,
				KEY_UNKNOWN_69 = 198,
				KEY_UNKNOWN_70 = 199,
				KEY_UNKNOWN_71 = 200,
				KEY_UNKNOWN_72 = 201,
				KEY_UNKNOWN_73 = 202,
				KEY_UNKNOWN_74 = 203,
				KEY_UNKNOWN_75 = 204,
				KEY_UNKNOWN_76 = 205,
				KEY_UNKNOWN_77 = 206,
				KEY_UNKNOWN_78 = 207,
				KEY_UNKNOWN_79 = 208,
				KEY_UNKNOWN_80 = 209,
				KEY_UNKNOWN_81 = 210,
				KEY_UNKNOWN_82 = 211,
				KEY_UNKNOWN_83 = 212,
				KEY_UNKNOWN_84 = 213,
				KEY_UNKNOWN_85 = 214,

				KEY_MODIFIERS = 215,

				KEY_LSHIFT = 215,
				KEY_RSHIFT = 216,
				KEY_LCTRL = 217,
				KEY_RCTRL = 218,
				KEY_ALT = 219,
				KEY_ALTGR = 220,
				KEY_LWIN = 221,
				KEY_RWIN = 222,
				KEY_MENU = 223,
				KEY_SCROLLLOCK = 224,
				KEY_NUMLOCK = 225,
				KEY_CAPSLOCK = 226,

				MOUSE_0,
				MOUSE_1,
				MOUSE_2,
				MOUSE_3,
				MOUSE_4,
				MOUSE_5,
				MOUSE_6,
				MOUSE_7,
				MOUSE_8,
				MOUSE_9,

				CUSTOMEVENT_DISPLAY_TOGGLED,
				CUSTOMEVENT_DISPLAY_RESIZED

			};
			enum event_xy{X,Y};

// ARG TYPE, ID NUMBER (FOR SEARCH LATER)
#define NEWF(x,y) (LSW::v2::Events::_functionThr*)new LSW::v2::Events::functionC<x>(y)

			class _functionThr {
			protected:
				int id = 0;
				double pertime = 1.0 / 120; // run x times per sec
				double _lasttry = 0; // do not change, keeps last call
				double f = 0.0;
				void(*fu) (void*, double&);
			public:
				void setId(const int);
				const int getId();
				void setUpdateTime(const double); // tps
				void resetTo(const double); // tps
				const bool _gotWork();
				double getVal();
				void setF(void(*) (void*, double&));

				virtual void work() = 0;
				virtual void* getArgAsVoid() = 0;
			};

			template <typename T>
			class functionC : _functionThr {
				T arg;
			public:
				functionC<T>(const int v) {
					id = v;
				}
				T& g()
				{
					return arg;
				}
				void work()
				{
					if (fu && _gotWork()) fu((void*)&arg, f);
				}
				void* getArgAsVoid() {
					return (void*)&arg;
				}
			};

			struct add_t_s {
				//double* f = nullptr;
				double adding = 0.01; // add per time
			};
			struct cos_t_s {
				double default_val = 1.0;
				double ampl = 0.2; // 0.8 to 1.2
				double time_multiplier = 1.0; // for the cos arg, cos(.. * this)
			};


			class _string_work {
				int lastkey = 0;
				int input = 0;

				Safer::safe_string done, ongoing;

				const char translate(const int);
			public:
				void interpret(int, int);
				void getLastString(Safer::safe_string&);
				void getOnGoingString(Safer::safe_string&);
				const bool isHack(hack_strings&);
				void clear();
				void clearall();
			};
			
			struct _event_log {
				double _collisionTimer_last = 0.0;
				double _functionsTimer_last = 0.0;
				//double _checkEnd_Timer_last = 0.0;
				double _calcLoopsTimer_last = 0.0;
				double _updatePosTimer_last = 0.0;
				unsigned _loops = 0;

				double collisionTimer_tps = 0.0;
				double functionsTimer_tps = 0.0;
				//double checkEnd_Timer_tps = 0.0;
				double calcLoopsTimer_tps = 0.0;
				double updatePosTimer_tps = 0.0;

				unsigned loops_per_second = 0;
			};


			class events {
				Log::gfile glog;
				std::thread* thr = nullptr;
				bool thr_running_yet = false;
				//unsigned ticks_one_sec = 0;
				ALLEGRO_DISPLAY* targ = nullptr;
				ALLEGRO_EVENT_QUEUE* ev_qu = nullptr;
				ALLEGRO_EVENT ev;

				Safer::safe_string lastString, currString;

				Safer::safe_vector<_functionThr*> funcs;

				std::map<events_keys, bool> keys;
				std::map<int, ALLEGRO_TIMER*> timers;

				float axis[2] = { 0 };

				double multiplier_hz_d = 1.0 / 60;

				bool alright = false;

				_event_log last_log;

				_functionThr* _get(const int);
			public:
				const bool setup();
				void stop();

				bool* getKeep();
				void setMultiplierForUpdatingImg(const double); // like 144 for 144 hz, has to be before everything

				const bool addFunction(_functionThr*, void(*)(void*, double&)); // IF BOOL = TRUE, RUN, else id
				void delFunction(const int);
				void delAllFunctions();
				double getFunctionValNow(const int);
				void* getFVoidArg(const int);
				void setTimerForF(const double, const int);
				void resetFValTo(const double, const int);

				ALLEGRO_TIMER* getTimer(const int, const double = -1);
				void setRunningTimer(const int, const bool);
				void delTimer(const int);

				const bool getKey(const events_keys, const bool = true); // auto reset key to false?
				void getMouse(float&, const event_xy);
				const bool getLastString(Safer::safe_string&, const bool = false); // clear?
				const bool getCurrentString(Safer::safe_string&, const bool = false); // clear?
				//const unsigned getCurrentMaxTickCounted();

				const bool _nextEv(ALLEGRO_EVENT&, const bool = false); // wait?
				void _setKey(const events_keys, const bool);
				void _confirm_initialized_thr();
				const bool _keep();
				void _update_functions();
				void _setMousePos(const float, const float);
				void _setLastString(const Safer::safe_string);
				void _setCurrString(const Safer::safe_string);
				//void _setMaxTickCount(const unsigned);
				const double _getMultiplierForUpdatingImg();
				ALLEGRO_DISPLAY* _getTargD();
				void _setThrRunning(const bool);

				_event_log& _getEventLog();
				//void _setEventLog(const _event_log&);

				void _setShutDownNow();
				const bool isSafeToQuit();
			};

			class big_event {
				static events* eventr;
			public:
				~big_event();
				void initInstance();
				void initSetupAndRun();

				events& g();
			};


			void _thread_ev(events* = nullptr);


			void defaultfunction_add(void*, double&);
			void defaultfunction_cos(void*, double&);


			void _i_thr_collisionTimed(_event_log*, events*, double, bool*);
			void _i_thr_functionsTimed(_event_log*, events*, double, bool*);
			void _i_thr_updatePosTimed(_event_log*, events*, double, bool*);
		}
	}
}
