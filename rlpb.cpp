#include "pch.h"
#include "rlpb.h"
#include "ItemStructs.h"


BAKKESMOD_PLUGIN(rlpb, "Twitch Preset Integration", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

bool coolEnabled = false;

void rlpb::onLoad() {
	_globalCvarManager = cvarManager;
	LOG("Plugin loaded!");

	
	cvarManager->registerNotifier("BetterBallOnTop", [this](std::vector<std::string> args) {
		ballOnTop(); }, "", PERMISSION_ALL);

	cvarManager->registerNotifier("spitPreset", [this](std::vector<std::string> args) {
		spitPreset(); }, "", PERMISSION_ALL);

	cvarManager->registerCvar("rlpb_token", "", "Token for loadout data", false);
	cvarManager->registerCvar("rlpb_enabled", "0", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
			coolEnabled = cvar.getBoolValue();
		});
	cvarManager->registerCvar("rlpb_distance", "200.0", "Distance to place the ball above");

	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("rlpb debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	LOG("the cvar with name: {} changed", cvarName);
	//	LOG("the new value is: {}", newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&rlpb::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&rlpb::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&rlpb::YourPluginMethod, this);
}

void rlpb::onUnload() {
	LOG("I was too cool for this world B'(");
}

void rlpb::ballOnTop() {
	if (!gameWrapper->IsInFreeplay()) { return; }

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	if (!coolEnabled) { return; }

	BallWrapper ball = server.GetBall();
	if (!ball) { return; }
	
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { return; }

	CVarWrapper distanceCVar = cvarManager->getCvar("rlpb_distance");
	if (!distanceCVar) { return; }
	float distance = distanceCVar.getFloatValue();

	Vector carVelocity = car.GetVelocity();
	ball.SetVelocity(carVelocity);

	Vector carLocation = car.GetLocation();
	float ballRadius = ball.GetRadius();
	ball.SetLocation(carLocation + Vector{0, 0, distance });

}

void rlpb::spitPreset() {
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { LOG("CarWrapper nono");  return; }
	LOG("GetLoadoutTeamIndex:");
	LOG(std::to_string(car.GetLoadoutTeamIndex()));

	LOG("GetPreviewTeamIndex:");
	LOG(std::to_string(car.GetPreviewTeamIndex()));

	LOG("GetbLoadoutSet:");
	LOG(std::to_string(car.GetbLoadoutSet()));

	LOG("GetLoadoutBody:");
	LOG(std::to_string(car.GetLoadoutBody()));

	LOG("GetOwnerName:");
	LOG(car.GetOwnerName());

	ItemsWrapper items = gameWrapper->GetItemsWrapper();
	if (!items) { LOG("ItemsWrapper nono");  return; }

	LoadoutWrapper loadout = items.GetCurrentLoadout(car.GetPreviewTeamIndex());

	LOG("loadout.GetLoadout().Count()");
	LOG(std::to_string(loadout.GetLoadout().Count()));

	LOG("loadout.GetLoadout().IsNull()");
	LOG(std::to_string(loadout.GetLoadout().IsNull()));

	LOG("loadout.GetPrimaryPaintColorId()");
	LOG(std::to_string(loadout.GetPrimaryPaintColorId()));

	LOG("loadout.GetAccentPaintColorId()");
	LOG(std::to_string(loadout.GetAccentPaintColorId()));

	LOG("loadout.GetPrimaryFinishId()");
	LOG(std::to_string(loadout.GetPrimaryFinishId()));

	LOG("loadout.GetAccentFinishId()");
	LOG(std::to_string(loadout.GetAccentFinishId()));

	auto load = loadout.GetOnlineLoadoutV2();

	LOG("InstanceId: {}: {}-{}.", "Body", load[0].lower_bits, load[0].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Decal", load[1].lower_bits, load[1].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Wheels", load[2].lower_bits, load[2].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Boost", load[3].lower_bits, load[3].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Antenna", load[4].lower_bits, load[4].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Topper", load[5].lower_bits, load[5].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Paint", load[6].lower_bits, load[6].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "PaintAlt", load[12].lower_bits, load[12].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "EngineSound", load[13].lower_bits, load[13].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "Trail", load[14].lower_bits, load[14].upper_bits);
	LOG("InstanceId: {}: {}-{}.", "GoalExplosion", load[15].lower_bits, load[15].upper_bits);
}