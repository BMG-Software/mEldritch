# Makefile in Eldritch

PROGRAM=$(ELDRITCH)$(EXE)

CXXFILES=src/Actions/wbactioneldaddblock.cpp src/Actions/wbactioneldbanktransaction.cpp \
	src/Actions/wbactioneldbindinput.cpp src/Actions/wbactioneldcheckcone.cpp \
	src/Actions/wbactioneldcheckline.cpp src/Actions/wbactioneldchecksphere.cpp \
	src/Actions/wbactionelddamageworld.cpp src/Actions/wbactioneldgiveitem.cpp \
	src/Actions/wbactioneldgotolevel.cpp src/Actions/wbactioneldlaunchwebsite.cpp \
	src/Actions/wbactioneldplayanim.cpp src/Actions/wbactioneldplayhandanim.cpp \
	src/Actions/wbactioneldremoveblock.cpp src/Actions/wbactioneldsetlight.cpp \
	src/Actions/wbactioneldsetpersistentvar.cpp src/Actions/wbactioneldshowbook.cpp \
	src/Actions/wbactioneldspawnentity.cpp src/Actions/wbactioneldstopmotion.cpp \
	src/Actions/wbactioneldtweetrip.cpp src/AnimEvents/animeventeldexecuteaction.cpp \
	src/AnimEvents/animeventeldsendevent.cpp src/BTNodes/rodinbtnodeeldlookat.cpp \
	src/BTNodes/rodinbtnodeeldmoveto.cpp src/BTNodes/rodinbtnodeeldplayanim.cpp \
	src/BTNodes/rodinbtnodeeldplaybark.cpp src/BTNodes/rodinbtnodeeldstopmoving.cpp \
	src/BTNodes/rodinbtnodeeldturntoward.cpp src/Components/wbcompeldaimotion.cpp \
	src/Components/wbcompeldammobag.cpp src/Components/wbcompeldanchor.cpp \
	src/Components/wbcompeldcamera.cpp src/Components/wbcompeldcharacter.cpp \
	src/Components/wbcompeldclimbable.cpp src/Components/wbcompeldcollision.cpp \
	src/Components/wbcompelddamager.cpp src/Components/wbcompelddoor.cpp \
	src/Components/wbcompelddropshadow.cpp src/Components/wbcompeldendgamecounter.cpp \
	src/Components/wbcompeldfaction.cpp src/Components/wbcompeldfootsteps.cpp \
	src/Components/wbcompeldfrobbable.cpp src/Components/wbcompeldfrobber.cpp \
	src/Components/wbcompeldhands.cpp src/Components/wbcompeldhard.cpp \
	src/Components/wbcompeldhardlistener.cpp src/Components/wbcompeldheadtracker.cpp \
	src/Components/wbcompeldhealth.cpp src/Components/wbcompeldhudmarker.cpp \
	src/Components/wbcompeldicicles.cpp src/Components/wbcompeldinventory.cpp \
	src/Components/wbcompelditem.cpp src/Components/wbcompeldkeyring.cpp \
	src/Components/wbcompeldlight.cpp src/Components/wbcompeldlock.cpp \
	src/Components/wbcompeldmapmarker.cpp src/Components/wbcompeldmarkup.cpp \
	src/Components/wbcompeldmesh.cpp src/Components/wbcompeldminimap.cpp \
	src/Components/wbcompeldparticles.cpp src/Components/wbcompeldpickup.cpp \
	src/Components/wbcompeldplayer.cpp src/Components/wbcompeldpowerteleport.cpp \
	src/Components/wbcompeldrespawner.cpp src/Components/wbcompeldrope.cpp \
	src/Components/wbcompeldsensor.cpp src/Components/wbcompeldsensordamage.cpp \
	src/Components/wbcompeldsensorhearing.cpp src/Components/wbcompeldsensormarkup.cpp \
	src/Components/wbcompeldsensorpoll.cpp src/Components/wbcompeldsensortheft.cpp \
	src/Components/wbcompeldsensorvision.cpp src/Components/wbcompeldsleeper.cpp \
	src/Components/wbcompeldsound.cpp src/Components/wbcompeldspikes.cpp \
	src/Components/wbcompeldthinker.cpp src/Components/wbcompeldthinkerhearwormtarget.cpp \
	src/Components/wbcompeldthinkerpatrol.cpp src/Components/wbcompeldthinkertarget.cpp \
	src/Components/wbcompeldtransform.cpp src/Components/wbcompeldtrapbolt.cpp \
	src/Components/wbcompeldusable.cpp src/Components/wbcompeldvisible.cpp \
	src/Components/wbcompeldwallet.cpp src/Components/wbcompeldwatson.cpp \
	src/Components/wbcompeldweapon.cpp src/Components/wbeldritchcomponent.cpp \
	src/eldritchbank.cpp src/eldritchfactions.cpp src/eldritchframework.cpp \
	src/eldritchgame.cpp src/eldritchirradiance.cpp src/eldritchmesh.cpp \
	src/eldritchmusic.cpp src/eldritchnav.cpp src/eldritchparticles.cpp \
	src/eldritchpersistence.cpp src/eldritchroom.cpp src/eldritchsaveload.cpp \
	src/eldritchsound3dlistener.cpp src/eldritchtargetmanager.cpp src/eldritchtools.cpp \
	src/eldritchwbscenehelpers.cpp src/eldritchworld.cpp src/eldritchworldgen.cpp \
	src/main.cpp src/PEs/wbpeelddistance.cpp src/PEs/wbpeeldgetcharactervo.cpp \
	src/PEs/wbpeeldgetfaction.cpp src/PEs/wbpeeldgetitem.cpp src/PEs/wbpeeldgetlocation.cpp \
	src/PEs/wbpeeldgetpersistentvar.cpp src/PEs/wbpeeldgetslot.cpp src/PEs/wbpeeldgetvelocity.cpp \
	src/PEs/wbpeeldhardscalar.cpp src/PEs/wbpeeldhardswitch.cpp src/PEs/wbpeeldplayer.cpp \
	src/Screens/uiscreen-eldbindinputs.cpp src/Screens/uiscreen-eldcredits.cpp \
	src/Screens/uiscreen-eldmirror.cpp src/Screens/uiscreen-eldsetres.cpp \
	src/Screens/uiscreen-eldselectmod.cpp src/SDPs/sdpelddropshadow.cpp \
	src/SDPs/sdpeldhudcalibration.cpp src/SDPs/sdpeldhud.cpp src/SDPs/sdpeldlit.cpp \
	src/SDPs/sdpeldpost.cpp src/SDPs/sdpeldworld.cpp src/uiinputmapeldritch.cpp

INCLUDES=$(INC_CORE) $(INC_MATH) $(INC_2D) $(INC_3D) $(INC_FRAMEWORK) $(INC_WORKBENCH) \
	$(INC_RODIN) $(INC_AUDIO) $(INC_UI) $(INC_OPENAL)

LIBRARIES=$(LIB_WORKBENCH) $(LIB_FRAMEWORK) $(LIB_UI) $(LIB_AUDIO) $(LIB_RODIN) $(LIB_3D) \
	$(LIB_2D) $(LIB_CORE) $(LIB_MATH) $(LIB_ZLIB) $(LIB_GLEW)

EXTRALIBS=$(LIB_OPENAL) $(SYSLIBS) $(LIB_SDL2)

include $(TOP)/make_prg.mk
