Composite*			Code/Libraries/Workbench/src/Actions/wbactioncomposite.*
	NumActions=&
	@Action
	@@&=...
	...

Destroy*			Code/Libraries/Workbench/src/Actions/wbactiondestroy.*
	DestroyPE=...

EldAddBlock			Projects/Eldritch/src/Actions/wbactioneldaddblock.*
	Source=
	VoxelValue=

EldBankTransaction*	Projects/Eldritch/src/Actions/wbactioneldbanktransaction.*
	Amount=

EldBindInput		Projects/Eldritch/src/Actions/wbactioneldbindinput.*
	Input=


EldCheckCone*		Projects/Eldritch/src/Actions/wbactioneldcheckcone.*
	ConeAngle=
	ConeLength=
	CheckTag=

EldCheckLine*		Projects/Eldritch/src/Actions/wbactioneldcheckline.*
	LineLength=
	CheckTag=

EldCheckSphere*		Projects/Eldritch/src/Actions/wbactioneldchecksphere.*
	Radius=
	CheckTag=

EldDamageWorld		Projects/Eldritch/src/Actions/wbactionelddamageworld.*
	Radius=

EldGiveItem*		Projects/Eldritch/src/Actions/wbactioneldgiveitem.*
	Item=
	ItemPE=
	GiveTo=

EldGoToLevel*		Projects/Eldritch/src/Actions/wbactioneldgotolevel.*
	|GoToNextLevel=bool	# go to next level
	|GoToPrevLevel=bool	# go to prev level
	|ReturnToHub=bool	# return to hub
	|Level="LevelName"	# go to level LevelName

	Options:
	Restart=bool		# Re-roll worlds, leave hub intact
	FullRestart=bool	# Re-roll worlds including hub
	Immediate=bool		# Load new world immediately instead of on next tick; only safe outside world tick


EldLaunchWebSite*	Projects/Eldritch/src/Actions/wbactioneldlaunchwebsite.*
	-

EldPlayAnim*		Projects/Eldritch/src/Actions/wbactioneldplayanim.*
	Animation=
	Loop=
	PlayRatePE=

EldPlayHandAnim		Projects/Eldritch/src/Actions/wbactioneldplayhandanim.*
	Animation=

EldRemoveBlock		Projects/Eldritch/src/Actions/wbactioneldremoveblock.*
	Source=

EldSetLight			Projects/Eldritch/src/Actions/wbactioneldsetlight.*
	AddLight=

EldSetPersistentVar*	Projects/Eldritch/src/Actions/wbactioneldsetpersistentvar.*
	Key=
	ValuePE=

EldShowBook*		Projects/Eldritch/src/Actions/wbactioneldshowbook.*
	BookString=
	BookStringPE=
	IsDynamic=bool

EldSpawnEntity*		Projects/Eldritch/src/Actions/wbactioneldspawnentity.*
	Entity=
	EntityPE=
	LocationOverridePE=
	OrientationOverridePE=
	YawOnly=bool
	SpawnImpulseZ=float
	SpawnImpulse=float
	SpawnImpulsePE=
	SpawnOffsetZ=float

EldStopMotion		Projects/Eldritch/src/Actions/wbactioneldstopmotion.*
	-

EldTweetRIP*		Projects/Eldritch/src/Actions/wbactioneldtweetrip.*
	-

Log					Code/Libraries/Workbench/src/Actions/wbactionlog.*
	Text=

RodinBlackboardWrite	Code/Libraries/Rodin/src/Actions/wbactionrodinblackboardwrite.cpp
	BlackboardKey=
	ValuePE=

Selector*			Code/Libraries/Workbench/src/Actions/wbactionselector.*
	NumSelections=&
	@Selection
	@@&Condition=
	@@^Action=

SendEvent*			Code/Libraries/Workbench/src/Actions/wbactionsendevent.*
	EventName=
	QueueEvent=bool
	LogEvent=bool
	DispatchDelay=float
	DispatchDelayPE=
	EventOwner=
	Recipient=
	NumParameters=&
	@Parameter
	@@&Name=
	@@^Value=

SetConfigVar*		Code/Libraries/Workbench/src/Actions/wbactionsetconfigvar.*
	VarContext=
	VarName=
	ValuePE=

SetVariable*		Code/Libraries/Workbench/src/Actions/wbactionsetvariable.*
	EntityPE=
	VariableName=
	ValuePE=

TriggerStatMod		Code/Libraries/Workbench/src/Actions/wbactiontriggerstatmod.*
	StatModEvent=
	Trigger=bool

UIPushScreen*		Code/Libraries/UI/src/Actions/wbactionuipushscreen.*
	Screen=

UISetWidgetImage	Code/Libraries/UI/src/Actions/wbactionuisetwidgetimage.*
	Screen=
	Widget=
	Image=
	InagePE=

UIShowHideWidget*	Code/Libraries/UI/src/Actions/wbactionuishowhidewidget.*
	Screen=
	Widget=
	Hidden=bool
	SetDisabled=bool

UIShowYesNoDialog*	Code/Libraries/UI/src/Actions/wbactionuishowyesnodialog.*
	YesNoString=
	NumYesActions=&
	@YesAction
	@@&=

UnqueueEvent		Code/Libraries/Workbench/src/Actions/wbactionunqueueevent.*
	EventOwner=
	VariableMapTag=
