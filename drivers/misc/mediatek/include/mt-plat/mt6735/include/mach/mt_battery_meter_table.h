#ifndef _CUST_BATTERY_METER_TABLE_H
#define _CUST_BATTERY_METER_TABLE_H

#define BAT_NTC_10 0
#define BAT_NTC_47 0
#define BAT_NTC_68 1


#if (BAT_NTC_10 == 1)
#define RBAT_PULL_UP_R    16900
#endif

#if (BAT_NTC_47 == 1)
#define RBAT_PULL_UP_R    61900
#endif

#ifdef CONFIG_HQ_PROJECT_ZAL1066_LG
	#if (BAT_NTC_68 == 1)
	#define RBAT_PULL_UP_R    82000
	#endif
#endif

#define RBAT_PULL_UP_VOLT 1800

typedef struct _BATTERY_PROFILE_STRUCT {
	signed int percentage;
	signed int voltage;
} BATTERY_PROFILE_STRUCT, *BATTERY_PROFILE_STRUCT_P;

typedef struct _R_PROFILE_STRUCT {
	signed int resistance;
	signed int voltage;
} R_PROFILE_STRUCT, *R_PROFILE_STRUCT_P;

typedef enum {
	T1_0C,
	T2_25C,
	T3_50C
} PROFILE_TEMPERATURE;

#if (BAT_NTC_68 == 1)
BATT_TEMPERATURE Batt_Temperature_Table[] = {
		  {-20, 738978}, 
		  {-15, 547456}, 
		  {-10, 409600}, 
		  {-5, 309217},  
		  {0, 235606},	 
		  {5, 180980},	 
		  {10, 140138},  
		  {15, 109344},  
		  {20, 85929},	 
		  {25, 68000},	 
		  {30, 54167},	 
		  {35, 43421},	 
		  {40, 35016},	 
		  {45, 28406},	 
		  {50, 23166},   
		  {55, 18997},  
		  {60, 15657},   
		  {65, 12967},   
		  {70, 10794}    				   				   
};
#endif


#if (BAT_NTC_10 == 1)
BATT_TEMPERATURE Batt_Temperature_Table[] = {
	{-20, 68237},
	{-15, 53650},
	{-10, 42506},
	{-5, 33892},
	{0, 27219},
	{5, 22021},
	{10, 17926},
	{15, 14674},
	{20, 12081},
	{25, 10000},
	{30, 8315},
	{35, 6948},
	{40, 5834},
	{45, 4917},
	{50, 4161},
	{55, 3535},
	{60, 3014}
};
#endif

#if (BAT_NTC_47 == 1)
	BATT_TEMPERATURE Batt_Temperature_Table[] = {
	{-20, 483954},
	{-15, 360850},
	{-10, 271697},
	{-5, 206463},
	{0, 158214},
	{5, 122259},
	{10, 95227},
	{15, 74730},
	{20, 59065},
	{25, 47000},
	{30, 37643},
	{35, 30334},
	{40, 24591},
	{45, 20048},
	{50, 16433},
	{55, 13539},
	{60, 11210}
	};
#endif

#if 0//defined(CONFIG_MTK_HAFG_20)
/* T0 -10C */
BATTERY_PROFILE_STRUCT battery_profile_t0[] = {
	{0, 4048},
	{2, 4008},
	{4, 3989},
	{5, 3977},
	{7, 3966},
	{9, 3960},
	{11, 3950},
	{12, 3946},
	{14, 3938},
	{16, 3932},
	{18, 3926},
	{19, 3918},
	{21, 3910},
	{23, 3901},
	{25, 3894},
	{26, 3885},
	{28, 3874},
	{30, 3866},
	{32, 3856},
	{33, 3846},
	{35, 3838},
	{37, 3830},
	{39, 3823},
	{40, 3817},
	{42, 3814},
	{44, 3808},
	{46, 3806},
	{47, 3803},
	{49, 3801},
	{51, 3798},
	{53, 3795},
	{54, 3796},
	{56, 3795},
	{58, 3792},
	{60, 3792},
	{61, 3790},
	{63, 3789},
	{65, 3787},
	{67, 3785},
	{68, 3783},
	{70, 3781},
	{72, 3776},
	{74, 3772},
	{75, 3767},
	{77, 3763},
	{79, 3758},
	{81, 3751},
	{82, 3742},
	{84, 3734},
	{86, 3725},
	{88, 3719},
	{90, 3715},
	{91, 3712},
	{93, 3707},
	{95, 3702},
	{97, 3696},
	{98, 3678},
	{100, 3647},
	{101, 3612},
	{102, 3575},
	{103, 3537},
	{103, 3502},
	{104, 3472},
	{104, 3443},
	{104, 3419},
	{105, 3395},
	{105, 3373},
	{105, 3357},
	{105, 3341},
	{105, 3328},
	{105, 3317},
	{105, 3307},
	{105, 3300},
	{105, 3293},
	{105, 3288},
	{105, 3283},
	{105, 3275},
	{105, 3271},
	{105, 3267},
	{105, 3260},
	{106, 3256},
	{106, 3251},
	{106, 3243},
	{106, 3239},
	{106, 3233},
	{106, 3225},
	{106, 3218},
	{106, 3214},
	{106, 3209},
	{106, 3202},
	{106, 3196},
	{106, 3185},
	{106, 3171},
	{106, 3157},
	{106, 3142},
	{106, 3125},
	{106, 3114},
	{106, 3095},
	{106, 3095},
	{106, 3095}
};

/* T1 0C */
BATTERY_PROFILE_STRUCT battery_profile_t1[] = {
	{0, 4048},
	{2, 4008},
	{3, 3989},
	{5, 3977},
	{7, 3966},
	{8, 3960},
	{10, 3956},
	{11, 3951},
	{13, 3948},
	{15, 3941},
	{16, 3935},
	{18, 3928},
	{20, 3922},
	{21, 3914},
	{23, 3906},
	{25, 3898},
	{26, 3892},
	{28, 3882},
	{29, 3872},
	{31, 3860},
	{33, 3849},
	{34, 3839},
	{36, 3831},
	{38, 3824},
	{39, 3818},
	{41, 3815},
	{43, 3808},
	{44, 3805},
	{46, 3803},
	{47, 3798},
	{49, 3796},
	{51, 3793},
	{52, 3792},
	{54, 3790},
	{56, 3790},
	{57, 3788},
	{59, 3788},
	{60, 3787},
	{62, 3787},
	{64, 3785},
	{65, 3785},
	{67, 3784},
	{69, 3782},
	{70, 3779},
	{72, 3777},
	{74, 3774},
	{75, 3769},
	{77, 3766},
	{79, 3762},
	{80, 3756},
	{82, 3748},
	{83, 3742},
	{85, 3734},
	{87, 3724},
	{88, 3714},
	{90, 3708},
	{92, 3703},
	{93, 3701},
	{95, 3699},
	{97, 3696},
	{98, 3689},
	{100, 3662},
	{101, 3601},
	{103, 3533},
	{104, 3475},
	{104, 3418},
	{105, 3363},
	{105, 3315},
	{105, 3270},
	{105, 3238},
	{105, 3208},
	{105, 3191},
	{106, 3172},
	{106, 3159},
	{106, 3150},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137},
	{106, 3137}
};

/* T2 25C */
BATTERY_PROFILE_STRUCT battery_profile_t2[] = {
	{0, 4165},
	{1, 4149},
	{3, 4136},
	{4, 4121},
	{6, 4110},
	{7, 4098},
	{8, 4086},
	{10, 4081},
	{11, 4077},
	{13, 4067},
	{14, 4047},
	{15, 4025},
	{17, 4006},
	{18, 3993},
	{19, 3983},
	{21, 3975},
	{22, 3971},
	{24, 3968},
	{25, 3964},
	{26, 3958},
	{28, 3949},
	{29, 3943},
	{31, 3934},
	{32, 3928},
	{33, 3920},
	{35, 3913},
	{36, 3906},
	{38, 3898},
	{39, 3890},
	{40, 3878},
	{42, 3865},
	{43, 3853},
	{44, 3843},
	{46, 3836},
	{47, 3829},
	{49, 3824},
	{50, 3820},
	{51, 3814},
	{53, 3812},
	{54, 3807},
	{56, 3803},
	{57, 3801},
	{58, 3796},
	{60, 3794},
	{61, 3791},
	{62, 3789},
	{64, 3786},
	{65, 3784},
	{67, 3782},
	{68, 3781},
	{69, 3779},
	{71, 3779},
	{72, 3777},
	{74, 3775},
	{75, 3772},
	{76, 3769},
	{78, 3765},
	{79, 3761},
	{81, 3757},
	{82, 3752},
	{83, 3747},
	{85, 3741},
	{86, 3733},
	{87, 3724},
	{89, 3717},
	{90, 3706},
	{92, 3697},
	{93, 3695},
	{94, 3694},
	{96, 3692},
	{97, 3690},
	{99, 3684},
	{100, 3651},
	{101, 3587},
	{103, 3498},
	{104, 3347},
	{105, 3207},
	{105, 3164},
	{105, 3128},
	{105, 3087},
	{105, 3063},
	{105, 3041},
	{105, 3029},
	{105, 3026},
	{105, 3023},
	{105, 3005},
	{105, 2998},
	{106, 2992},
	{106, 2981},
	{106, 2973},
	{106, 2974},
	{106, 2975},
	{106, 2960},
	{106, 2950},
	{106, 2949},
	{106, 2947},
	{106, 2944},
	{106, 2939},
	{106, 2936},
	{106, 2931}
};

/* T3 50C */
BATTERY_PROFILE_STRUCT battery_profile_t3[] = {
	{0, 4181},
	{1, 4167},
	{3, 4152},
	{4, 4139},
	{5, 4127},
	{7, 4114},
	{8, 4103},
	{10, 4090},
	{11, 4078},
	{12, 4067},
	{14, 4056},
	{15, 4049},
	{16, 4036},
	{18, 4022},
	{19, 4010},
	{20, 4001},
	{22, 3995},
	{23, 3986},
	{25, 3977},
	{26, 3969},
	{27, 3959},
	{29, 3952},
	{30, 3943},
	{31, 3935},
	{33, 3929},
	{34, 3920},
	{35, 3913},
	{37, 3906},
	{38, 3899},
	{40, 3893},
	{41, 3887},
	{42, 3879},
	{44, 3867},
	{45, 3851},
	{46, 3840},
	{48, 3833},
	{49, 3827},
	{50, 3820},
	{52, 3816},
	{53, 3812},
	{55, 3808},
	{56, 3803},
	{57, 3800},
	{59, 3797},
	{60, 3794},
	{61, 3791},
	{63, 3787},
	{64, 3785},
	{65, 3782},
	{67, 3779},
	{68, 3778},
	{70, 3776},
	{71, 3775},
	{72, 3772},
	{74, 3767},
	{75, 3759},
	{76, 3753},
	{78, 3751},
	{79, 3746},
	{81, 3742},
	{82, 3737},
	{83, 3732},
	{85, 3729},
	{86, 3724},
	{87, 3715},
	{89, 3708},
	{90, 3699},
	{91, 3689},
	{93, 3681},
	{94, 3680},
	{95, 3680},
	{97, 3678},
	{98, 3676},
	{100, 3664},
	{101, 3619},
	{102, 3553},
	{104, 3454},
	{105, 3279},
	{106, 3141},
	{106, 3081},
	{106, 3038},
	{106, 3012},
	{106, 2982},
	{106, 2976},
	{106, 2956},
	{106, 2947},
	{106, 2942},
	{106, 2936},
	{106, 2939},
	{106, 2926},
	{106, 2925},
	{106, 2922},
	{106, 2918},
	{106, 2910},
	{106, 2904},
	{106, 2897},
	{106, 2891},
	{106, 2881},
	{106, 2873},
	{106, 2876}
};
#else
/* T0 -10C */
BATTERY_PROFILE_STRUCT battery_profile_t0[] = {
{0  ,4369},
{1  ,4345},
{2  ,4325},
{3  ,4308},
{4  ,4293},
{5  ,4277},
{6  ,4263},
{7  ,4249},
{8  ,4235},
{9  ,4222},
{11 ,4209},
{12 ,4196},
{13 ,4184},
{14 ,4171},
{15 ,4159},
{16 ,4146},
{17 ,4134},
{18 ,4121},
{19 ,4109},
{20 ,4098},
{21 ,4089},
{22 ,4078},
{23 ,4066},
{24 ,4052},
{25 ,4038},
{26 ,4023},
{27 ,4009},
{28 ,3995},
{30 ,3983},
{31 ,3971},
{32 ,3961},
{33 ,3952},
{34 ,3943},
{35 ,3935},
{36 ,3926},
{37 ,3918},
{38 ,3910},
{39 ,3902},
{40 ,3894},
{41 ,3886},
{42 ,3878},
{43 ,3870},
{44 ,3863},
{45 ,3856},
{46 ,3850},
{47 ,3843},
{49 ,3836},
{50 ,3831},
{51 ,3825},
{52 ,3819},
{53 ,3814},
{54 ,3809},
{55 ,3805},
{56 ,3801},
{57 ,3797},
{58 ,3793},
{59 ,3790},
{60 ,3788},
{61 ,3785},
{62 ,3782},
{63 ,3779},
{64 ,3777},
{65 ,3774},
{67 ,3771},
{68 ,3769},
{69 ,3766},
{70 ,3763},
{71 ,3761},
{72 ,3757},
{73 ,3754},
{74 ,3751},
{75 ,3748},
{76 ,3744},
{77 ,3741},
{78 ,3737},
{79 ,3734},
{80 ,3730},
{81 ,3726},
{82 ,3721},
{83 ,3717},
{85 ,3713},
{86 ,3708},
{87 ,3703},
{88 ,3698},
{89 ,3693},
{90 ,3687},
{91 ,3681},
{92 ,3674},
{93 ,3666},
{94 ,3657},
{95 ,3646},
{96 ,3638},
{97 ,3633},
{98 ,3626},
{99 ,3610},
{100,3590}


};

/* T1 0C */
BATTERY_PROFILE_STRUCT battery_profile_t1[] = {
 {0  , 4353 },
 {1  , 4333 },
 {2  , 4316 },
 {3  , 4301 },
 {4  , 4287 },
 {5  , 4273 },
 {6  , 4260 },
 {7  , 4247 },
 {8  , 4235 },
 {9  , 4222 },
 {10 , 4209 },
 {11 , 4197 },
 {12 , 4185 },
 {13 , 4173 },
 {14 , 4161 },
 {16 , 4150 },
 {17 , 4138 },
 {18 , 4126 },
 {19 , 4114 },
 {20 , 4103 },
 {21 , 4091 },
 {22 , 4081 },
 {23 , 4071 },
 {24 , 4061 },
 {25 , 4049 },
 {26 , 4036 },
 {27 , 4022 },
 {28 , 4008 },
 {29 , 3995 },
 {30 , 3983 },
 {31 , 3971 },
 {32 , 3960 },
 {33 , 3950 },
 {34 , 3941 },
 {35 , 3932 },
 {36 , 3924 },
 {37 , 3916 },
 {38 , 3907 },
 {39 , 3899 },
 {40 , 3890 },
 {41 , 3882 },
 {42 , 3874 },
 {43 , 3867 },
 {44 , 3859 },
 {45 , 3853 },
 {47 , 3846 },
 {48 , 3839 },
 {49 , 3833 },
 {50 , 3827 },
 {51 , 3822 },
 {52 , 3816 },
 {53 , 3811 },
 {54 , 3806 },
 {55 , 3801 },
 {56 , 3797 },
 {57 , 3792 },
 {58 , 3788 },
 {59 , 3784 },
 {60 , 3781 },
 {61 , 3778 },
 {62 , 3775 },
 {63 , 3773 },
 {64 , 3771 },
 {65 , 3768 },
 {66 , 3766 },
 {67 , 3764 },
 {68 , 3761 },
 {69 , 3759 },
 {70 , 3757 },
 {71 , 3754 },
 {72 , 3752 },
 {73 , 3748 },
 {74 , 3745 },
 {75 , 3742 },
 {77 , 3739 },
 {78 , 3735 },
 {79 , 3731 },
 {80 , 3726 },
 {81 , 3722 },
 {82 , 3717 },
 {83 , 3712 },
 {84 , 3707 },
 {85 , 3701 },
 {86 , 3695 },
 {87 , 3689 },
 {88 , 3682 },
 {89 , 3676 },
 {90 , 3669 },
 {91 , 3661 },
 {92 , 3653 },
 {93 , 3644 },
 {94 , 3632 },
 {95 , 3616 },
 {96 , 3593 },
 {97 , 3559 },
 {98 , 3527 },
 {99 , 3484 },
 {100 ,3395 }


};

/* T2 25C */
BATTERY_PROFILE_STRUCT battery_profile_t2[] = {
{0  ,4361}, 
{1  ,4341}, 
{2  ,4327}, 
{3  ,4313}, 
{4  ,4301}, 
{5  ,4288}, 
{6  ,4275}, 
{7  ,4263}, 
{8  ,4251}, 
{9  ,4239}, 
{10 ,4227}, 
{11 ,4214}, 
{12 ,4202}, 
{13 ,4190}, 
{14 ,4178}, 
{15 ,4166}, 
{16 ,4154}, 
{18 ,4142}, 
{19 ,4130}, 
{20 ,4119}, 
{21 ,4107}, 
{22 ,4096}, 
{23 ,4085}, 
{24 ,4073}, 
{25 ,4063}, 
{26 ,4053}, 
{27 ,4043}, 
{28 ,4031}, 
{29 ,4020}, 
{30 ,4008}, 
{31 ,3997}, 
{32 ,3987}, 
{33 ,3976}, 
{34 ,3967}, 
{35 ,3958}, 
{36 ,3948}, 
{37 ,3940}, 
{38 ,3930}, 
{39 ,3921}, 
{40 ,3911}, 
{41 ,3901}, 
{42 ,3892}, 
{43 ,3883}, 
{44 ,3874}, 
{45 ,3866}, 
{46 ,3859}, 
{47 ,3851}, 
{48 ,3845}, 
{49 ,3839}, 
{50 ,3833}, 
{52 ,3827}, 
{53 ,3821}, 
{54 ,3816}, 
{55 ,3811}, 
{56 ,3806}, 
{57 ,3801}, 
{58 ,3797}, 
{59 ,3792}, 
{60 ,3788}, 
{61 ,3784}, 
{62 ,3780}, 
{63 ,3777}, 
{64 ,3773}, 
{65 ,3770}, 
{66 ,3766}, 
{67 ,3764}, 
{68 ,3761}, 
{69 ,3759}, 
{70 ,3757}, 
{71 ,3755}, 
{72 ,3752}, 
{73 ,3750}, 
{74 ,3748}, 
{75 ,3745}, 
{76 ,3742}, 
{77 ,3739}, 
{78 ,3736}, 
{79 ,3732}, 
{80 ,3727}, 
{81 ,3723}, 
{82 ,3718}, 
{83 ,3712}, 
{85 ,3705}, 
{86 ,3698}, 
{87 ,3690}, 
{88 ,3682}, 
{89 ,3673}, 
{90 ,3667}, 
{91 ,3662}, 
{92 ,3658}, 
{93 ,3653}, 
{94 ,3647}, 
{95 ,3639}, 
{96 ,3627}, 
{97 ,3601}, 
{98 ,3556}, 
{99 ,3492}, 
{100,3405} 

	
};

/* T3 50C */
BATTERY_PROFILE_STRUCT battery_profile_t3[] = {
 {0  , 4360}, 
 {1  , 4343}, 
 {2  , 4328}, 
 {3  , 4314}, 
 {4  , 4301}, 
 {5  , 4288}, 
 {6  , 4275}, 
 {7  , 4263}, 
 {8  , 4250}, 
 {9  , 4238}, 
 {10 , 4226}, 
 {11 , 4213}, 
 {12 , 4201}, 
 {13 , 4189}, 
 {15 , 4177}, 
 {16 , 4165}, 
 {17 , 4153}, 
 {18 , 4142}, 
 {19 , 4130}, 
 {20 , 4119}, 
 {21 , 4107}, 
 {22 , 4096}, 
 {23 , 4085}, 
 {24 , 4074}, 
 {25 , 4063}, 
 {26 , 4053}, 
 {27 , 4042}, 
 {28 , 4032}, 
 {29 , 4022}, 
 {30 , 4012}, 
 {31 , 4002}, 
 {32 , 3993}, 
 {33 , 3984}, 
 {34 , 3975}, 
 {35 , 3966}, 
 {36 , 3958}, 
 {37 , 3950}, 
 {38 , 3942}, 
 {39 , 3933}, 
 {40 , 3925}, 
 {41 , 3917}, 
 {42 , 3909}, 
 {44 , 3900}, 
 {45 , 3890}, 
 {46 , 3878}, 
 {47 , 3866}, 
 {48 , 3856}, 
 {49 , 3848}, 
 {50 , 3841}, 
 {51 , 3834}, 
 {52 , 3829}, 
 {53 , 3823}, 
 {54 , 3818}, 
 {55 , 3814}, 
 {56 , 3810}, 
 {57 , 3805}, 
 {58 , 3801}, 
 {59 , 3797}, 
 {60 , 3793}, 
 {61 , 3789}, 
 {62 , 3786}, 
 {63 , 3782}, 
 {64 , 3779}, 
 {65 , 3776}, 
 {66 , 3774}, 
 {67 , 3771}, 
 {68 , 3768}, 
 {69 , 3766}, 
 {70 , 3762}, 
 {71 , 3760}, 
 {73 , 3755}, 
 {74 , 3749}, 
 {75 , 3743}, 
 {76 , 3740}, 
 {77 , 3737}, 
 {78 , 3733}, 
 {79 , 3728}, 
 {80 , 3723}, 
 {81 , 3719}, 
 {82 , 3715}, 
 {83 , 3710}, 
 {84 , 3704}, 
 {85 , 3696}, 
 {86 , 3688}, 
 {87 , 3681}, 
 {88 , 3672}, 
 {89 , 3663}, 
 {90 , 3660}, 
 {91 , 3658}, 
 {92 , 3655}, 
 {93 , 3651}, 
 {94 , 3647}, 
 {95 , 3639}, 
 {96 , 3619}, 
 {97 , 3577}, 
 {98 , 3522}, 
 {99 , 3451}, 
 {100, 3353} 

};
#endif

/* battery profile for actual temperature. The size should be the same as T1, T2 and T3 */
BATTERY_PROFILE_STRUCT battery_profile_temperature[] = {
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0}
};

#if 0//defined(CONFIG_MTK_HAFG_20)
/* T0 -10C */
R_PROFILE_STRUCT r_profile_t0[] = {
	{865, 4048},
	{865, 4008},
	{893, 3989},
	{915, 3977},
	{955, 3966},
	{1023, 3960},
	{1200, 3950},
	{1338, 3946},
	{1375, 3938},
	{1388, 3932},
	{1408, 3926},
	{1420, 3918},
	{1428, 3910},
	{1418, 3901},
	{1428, 3894},
	{1423, 3885},
	{1418, 3874},
	{1425, 3866},
	{1428, 3856},
	{1428, 3846},
	{1425, 3838},
	{1423, 3830},
	{1420, 3823},
	{1415, 3817},
	{1425, 3814},
	{1425, 3808},
	{1450, 3806},
	{1468, 3803},
	{1465, 3801},
	{1483, 3798},
	{1488, 3795},
	{1510, 3796},
	{1515, 3795},
	{1533, 3792},
	{1535, 3792},
	{1548, 3790},
	{1543, 3789},
	{1563, 3787},
	{1588, 3785},
	{1610, 3783},
	{1625, 3781},
	{1640, 3776},
	{1653, 3772},
	{1660, 3767},
	{1680, 3763},
	{1690, 3758},
	{1710, 3751},
	{1733, 3742},
	{1745, 3734},
	{1765, 3725},
	{1788, 3719},
	{1813, 3715},
	{1853, 3712},
	{1905, 3707},
	{1965, 3702},
	{2010, 3696},
	{2080, 3678},
	{2123, 3647},
	{2035, 3612},
	{1943, 3575},
	{1853, 3537},
	{1770, 3502},
	{1685, 3472},
	{1623, 3443},
	{1550, 3419},
	{1493, 3395},
	{1448, 3373},
	{1395, 3357},
	{1368, 3341},
	{1338, 3328},
	{1303, 3317},
	{1298, 3307},
	{1263, 3300},
	{1253, 3293},
	{1260, 3288},
	{1225, 3283},
	{1240, 3275},
	{1198, 3271},
	{1215, 3267},
	{1198, 3260},
	{1200, 3256},
	{1218, 3251},
	{1228, 3243},
	{1138, 3239},
	{1230, 3233},
	{1243, 3225},
	{1155, 3218},
	{1165, 3214},
	{1045, 3209},
	{1170, 3202},
	{1183, 3196},
	{1340, 3185},
	{1368, 3171},
	{1423, 3157},
	{1455, 3142},
	{1533, 3125},
	{1365, 3114},
	{1653, 3095},
	{1653, 3095},
	{1653, 3095}
};
#else
/* T0 -10C */
R_PROFILE_STRUCT r_profile_t0[] = {
{1315.3 ,4369 }, 
{1315.3 ,4345 }, 
{1316.8 ,4325 }, 
{1304.5 ,4308 }, 
{1297.5 ,4293 }, 
{1282.8 ,4277 }, 
{1270.3 ,4263 }, 
{1255.0 ,4249 }, 
{1242.3 ,4235 }, 
{1228.5 ,4222 }, 
{1219.3 ,4209 }, 
{1206.0 ,4196 }, 
{1196.8 ,4184 }, 
{1188.3 ,4171 }, 
{1179.8 ,4159 }, 
{1172.0 ,4146 }, 
{1166.5 ,4134 }, 
{1158.0 ,4121 }, 
{1153.3 ,4109 }, 
{1151.8 ,4098 }, 
{1151.8 ,4089 }, 
{1153.3 ,4078 }, 
{1154.3 ,4066 }, 
{1151.8 ,4052 }, 
{1149.5 ,4038 }, 
{1135.5 ,4023 }, 
{1133.0 ,4009 }, 
{1124.8 ,3995 }, 
{1118.5 ,3983 }, 
{1118.5 ,3971 }, 
{1115.3 ,3961 }, 
{1115.3 ,3952 }, 
{1116.8 ,3943 }, 
{1120.8 ,3935 }, 
{1115.5 ,3926 }, 
{1123.0 ,3918 }, 
{1123.0 ,3910 }, 
{1121.5 ,3902 }, 
{1126.3 ,3894 }, 
{1126.8 ,3886 }, 
{1123.0 ,3878 }, 
{1128.5 ,3870 }, 
{1135.5 ,3863 }, 
{1140.3 ,3856 }, 
{1141.8 ,3850 }, 
{1142.5 ,3843 }, 
{1147.0 ,3836 }, 
{1152.5 ,3831 }, 
{1158.0 ,3825 }, 
{1162.5 ,3819 }, 
{1169.5 ,3814 }, 
{1175.0 ,3809 }, 
{1180.3 ,3805 }, 
{1187.5 ,3801 }, 
{1194.5 ,3797 }, 
{1203.8 ,3793 }, 
{1217.8 ,3790 }, 
{1227.8 ,3788 }, 
{1244.0 ,3785 }, 
{1253.3 ,3782 }, 
{1269.5 ,3779 }, 
{1283.5 ,3777 }, 
{1300.5 ,3774 }, 
{1315.3 ,3771 }, 
{1335.5 ,3769 }, 
{1354.0 ,3766 }, 
{1375.0 ,3763 }, 
{1400.5 ,3761 }, 
{1411.5 ,3757 }, 
{1444.8 ,3754 }, 
{1464.8 ,3751 }, 
{1494.3 ,3748 }, 
{1522.3 ,3744 }, 
{1550.3 ,3741 }, 
{1579.5 ,3737 }, 
{1611.3 ,3734 }, 
{1642.3 ,3730 }, 
{1673.5 ,3726 }, 
{1704.3 ,3721 }, 
{1737.0 ,3717 }, 
{1773.5 ,3713 }, 
{1812.8 ,3708 }, 
{1846.8 ,3703 }, 
{1893.5 ,3698 }, 
{1941.5 ,3693 }, 
{1995.0 ,3687 }, 
{2067.0 ,3681 }, 
{2154.8 ,3674 }, 
{2272.3 ,3666 }, 
{2449.8 ,3657 }, 
{2702.8 ,3646 }, 
{2848.3 ,3639 }, 
{2842.3 ,3633 }, 
{2840   ,3623 }, 
{2830   ,3610 }, 
{2800   ,3590 }, 
{2755   ,3587 }, 
{2738   ,3584 } 

};
#endif

/* T1 0C */
R_PROFILE_STRUCT r_profile_t1[] = {
{657.3,  4353 }, 
{657.3  , 4333 },
{661.8  , 4316 },
{661.2  , 4301 },
{656.5  , 4287 },
{654.3  , 4273 },
{654.0  , 4260 },
{646.3  , 4247 },
{648.0  , 4235 },
{645.5  , 4222 },
{637.0  , 4209 },
{635.5  , 4197 },
{634.0  , 4185 },
{631.5  , 4173 },
{634.8  , 4161 },
{626.3  , 4150 },
{630.3  , 4138 },
{624.0  , 4126 },
{622.2  , 4114 },
{623.3  , 4103 },
{614.0  , 4091 },
{619.5  , 4081 },
{622.3  , 4071 },
{624.8  , 4061 },
{630.8  , 4049 },
{629.3  , 4036 },
{622.3  , 4022 },
{618.5  , 4008 },
{609.3  , 3995 },
{605.5  , 3983 },
{598.3  , 3971 },
{593.5  , 3960 },
{590.5  , 3950 },
{591.3  , 3941 },
{588.3  , 3932 },
{591.5  , 3924 },
{589.8  , 3916 },
{590.5  , 3907 },
{589.3  , 3899 },
{589.8  , 3890 },
{588.3  , 3882 },
{589.0  , 3874 },
{589.8  , 3867 },
{590.5  , 3859 },
{592.3  , 3853 },
{592.0  , 3846 },
{594.3  , 3839 },
{596.0  , 3833 },
{598.2  , 3827 },
{600.0  , 3822 },
{603.8  , 3816 },
{606.8  , 3811 },
{610.0  , 3806 },
{613.0  , 3801 },
{616.3  , 3797 },
{619.3  , 3792 },
{622.5  , 3788 },
{627.0  , 3784 },
{630.3  , 3781 },
{636.5  , 3778 },
{643.3  , 3775 },
{653.5  , 3773 },
{661.0  , 3771 },
{671.0  , 3768 },
{682.3  , 3766 },
{693.0  , 3764 },
{706.0  , 3761 },
{717.5  , 3759 },
{731.0  , 3757 },
{743.3  , 3754 },
{760.3  , 3752 },
{773.5  , 3748 },
{788.3  , 3745 },
{806.0  , 3742 },
{827.0  , 3739 },
{844.0  , 3735 },
{863.3  , 3731 },
{886.0  , 3726 },
{912.0  , 3722 },
{938.5  , 3717 },
{964.0  , 3712 },
{993.8  , 3707 },
{1024.8 , 3701 },
{1056.5 , 3695 },
{1096.0 , 3689 },
{1139.5 , 3682 },
{1189.0 , 3676 },
{1246.3 , 3669 },
{1312.3 , 3661 },
{1386.5 , 3653 },
{1478.0 , 3644 },
{1584.8 , 3632 },
{1718.3 , 3616 },
{1918.3 , 3593 },
{2285.8 , 3559 },
{2571.8 , 3527 },
{2525.3 , 3484 },
{2495.0  ,3395 } 

};

/* T2 25C */
R_PROFILE_STRUCT r_profile_t2[] = {
{211.8  , 4361 },
{211.8  , 4341 },
{212.2  , 4327 },
{211.8  , 4313 },
{211.5  , 4301 },
{210.0  , 4288 },
{210.0  , 4275 },
{210.0  , 4263 },
{211.5  , 4251 },
{212.2  , 4239 },
{212.5  , 4227 },
{212.2  , 4214 },
{214.0  , 4202 },
{214.0  , 4190 },
{214.8  , 4178 },
{214.0  , 4166 },
{215.5  , 4154 },
{216.2  , 4142 },
{217.0  , 4130 },
{218.5  , 4119 },
{217.7  , 4107 },
{218.5  , 4096 },
{218.8  , 4085 },
{219.3  , 4073 },
{219.5  , 4063 },
{220.7  , 4053 },
{222.5  , 4043 },
{223.0  , 4031 },
{224.8  , 4020 },
{224.0  , 4008 },
{224.8  , 3997 },
{223.3  , 3987 },
{223.3  , 3976 },
{222.5  , 3967 },
{223.2  , 3958 },
{221.8  , 3948 },
{221.0  , 3940 },
{221.0  , 3930 },
{219.5  , 3921 },
{217.3  , 3911 },
{215.3  , 3901 },
{214.5  , 3892 },
{210.8  , 3883 },
{209.3  , 3874 },
{207.8  , 3866 },
{207.0  , 3859 },
{206.8  , 3851 },
{206.0  , 3845 },
{206.0  , 3839 },
{206.0  , 3833 },
{206.8  , 3827 },
{207.0  , 3821 },
{207.8  , 3816 },
{208.5  , 3811 },
{207.8  , 3806 },
{208.5  , 3801 },
{209.3  , 3797 },
{210.0  , 3792 },
{210.7  , 3788 },
{211.8  , 3784 },
{211.5  , 3780 },
{212.3  , 3777 },
{212.5  , 3773 },
{214.0  , 3770 },
{213.3  , 3766 },
{214.0  , 3764 },
{214.0  , 3761 },
{215.5  , 3759 },
{216.3  , 3757 },
{217.8  , 3755 },
{220.0  , 3752 },
{221.0  , 3750 },
{221.8  , 3748 },
{223.3  , 3745 },
{224.0  , 3742 },
{225.5  , 3739 },
{229.5  , 3736 },
{231.0  , 3732 },
{232.5  , 3727 },
{235.8  , 3723 },
{240.3  , 3718 },
{243.2  , 3712 },
{245.0  , 3705 },
{249.0  , 3698 },
{253.5  , 3690 },
{258.0  , 3682 },
{260.5  , 3673 },
{266.8  , 3667 },
{273.5  , 3662 },
{286.0  , 3658 },
{300.0  , 3653 },
{318.5  , 3647 },
{338.0  , 3639 },
{364.3  , 3627 },
{389.0  , 3601 },
{426.3  , 3556 },
{488.3  , 3492 },
{593.8  , 3405 }

	
};

/* T3 50C */
R_PROFILE_STRUCT r_profile_t3[] = {
{175.0 , 4360 },
{175.0 , 4343 },
{171.3 , 4328 },
{172.0 , 4314 },
{171.3 , 4301 },
{170.5 , 4288 },
{169.7 , 4275 },
{169.7 , 4263 },
{169.0 , 4250 },
{170.5 , 4238 },
{169.0 , 4226 },
{169.7 , 4213 },
{170.5 , 4201 },
{169.7 , 4189 },
{171.5 , 4177 },
{172.3 , 4165 },
{172.0 , 4153 },
{172.8 , 4142 },
{172.8 , 4130 },
{174.5 , 4119 },
{174.5 , 4107 },
{175.0 , 4096 },
{176.0 , 4085 },
{176.8 , 4074 },
{178.3 , 4063 },
{177.5 , 4053 },
{179.0 , 4042 },
{180.7 , 4032 },
{180.5 , 4022 },
{182.0 , 4012 },
{183.0 , 4002 },
{183.8 , 3993 },
{184.5 , 3984 },
{186.8 , 3975 },
{187.5 , 3966 },
{188.5 , 3958 },
{190.7 , 3950 },
{191.3 , 3942 },
{192.2 , 3933 },
{193.8 , 3925 },
{195.5 , 3917 },
{196.0 , 3909 },
{195.3 , 3900 },
{193.0 , 3890 },
{185.3 , 3878 },
{179.0 , 3866 },
{173.8 , 3856 },
{172.0 , 3848 },
{169.8 , 3841 },
{169.0 , 3834 },
{169.7 , 3829 },
{169.8 , 3823 },
{170.5 , 3818 },
{172.0 , 3814 },
{172.3 , 3810 },
{172.8 , 3805 },
{173.8 , 3801 },
{174.5 , 3797 },
{175.8 , 3793 },
{176.8 , 3789 },
{178.3 , 3786 },
{178.2 , 3782 },
{179.0 , 3779 },
{180.5 , 3776 },
{181.3 , 3774 },
{182.0 , 3771 },
{182.8 , 3768 },
{183.8 , 3766 },
{182.3 , 3762 },
{182.3 , 3760 },
{179.0 , 3755 },
{175.0 , 3749 },
{172.0 , 3743 },
{174.3 , 3740 },
{177.5 , 3737 },
{177.5 , 3733 },
{177.2 , 3728 },
{176.8 , 3723 },
{177.3 , 3719 },
{179.8 , 3715 },
{180.8 , 3710 },
{180.8 , 3704 },
{180.5 , 3696 },
{181.5 , 3688 },
{182.8 , 3681 },
{182.8 , 3672 },
{179.8 , 3663 },
{181.3 , 3660 },
{186.0 , 3658 },
{190.8 , 3655 },
{195.5 , 3651 },
{201.5 , 3647 },
{205.5 , 3639 },
{202.3 , 3619 },
{204.8 , 3577 },
{211.8 , 3522 },
{226.3 , 3451 },
{251.0 , 3353 }


};

/* r-table profile for actual temperature. The size should be the same as T1, T2 and T3 */
R_PROFILE_STRUCT r_profile_temperature[] = {
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0}
};

/* ============================================================
// function prototype
// ============================================================*/
int fgauge_get_saddles(void);
BATTERY_PROFILE_STRUCT_P fgauge_get_profile(unsigned int temperature);

int fgauge_get_saddles_r_table(void);
R_PROFILE_STRUCT_P fgauge_get_profile_r_table(unsigned int temperature);

#endif	/*#ifndef _CUST_BATTERY_METER_TABLE_H*/

