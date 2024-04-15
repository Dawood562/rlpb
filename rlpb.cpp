#include "pch.h"
#include "rlpb.h"

#include "InventoryModel.h"
#include "utils/parser.h"


BAKKESMOD_PLUGIN(rlpb, "Twitch Preset Integration", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

bool coolEnabled = false;

void rlpb::onLoad() {
	_globalCvarManager = cvarManager;
	LOG("Plugin loaded!");

	auto inventoryModel = std::make_shared<InventoryModel>(gameWrapper);

	cvarManager->registerNotifier("BetterBallOnTop", [this](std::vector<std::string> args) {
		ballOnTop(); }, "", PERMISSION_ALL);

	cvarManager->registerNotifier("spitPreset", [&, this](std::vector<std::string> args) {
		spitPreset(inventoryModel); }, "", PERMISSION_ALL);

	cvarManager->registerCvar("rlpb_token", "", "Token for loadout data", false);
	cvarManager->registerCvar("rlpb_enabled", "0", "Enable Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		coolEnabled = cvar.getBoolValue();
			});
	cvarManager->registerCvar("rlpb_distance", "200.0", "Distance to place the ball above");

	gameWrapper->HookEvent("Function TAGame.Team_TA.PostBeginPlay",
		[&, this](std::string eventName) {
			// Function is called twice per match start, once per team that is totally joined and ready
			PostBeginPlayCounter++;
			if (PostBeginPlayCounter >= 2) {
				LOG("Function TAGame.Team_TA.PostBeginPlay happened! Twice.");
				PostBeginPlayCounter = 0;
			}
		});

	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("rlpb debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

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

void rlpb::spitPreset(std::shared_ptr<InventoryModel> im) {
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { LOG("CarWrapper nono");  return; }

	LOG("GetLoadoutBody:");
	LOG(std::to_string(car.GetLoadoutBody()));

	ItemsWrapper items = gameWrapper->GetItemsWrapper();
	if (!items) { LOG("ItemsWrapper nono");  return; }

	LoadoutWrapper loadout = items.GetCurrentLoadout(car.GetPreviewTeamIndex()); // 0 = Blue, 1 = Orange

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

	for (auto item : load)
	{
		auto data = im->GetProdData(item);
		gameWrapper->Execute([&, this](GameWrapper* gw) {
			LOG("InstanceId: {}:{}. slot: {}", item.lower_bits, item.upper_bits, im->GetSlotName(data.slot));
		});
	}

	
}