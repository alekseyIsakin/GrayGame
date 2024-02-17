#include "pch.hpp"
#include "engine.hpp"

GR::Entity sword;
GR::Entity gizmo;
std::vector<GR::Entity> entityes;
std::vector<int> entityesFlyOffset;
std::vector<TVec3> entityesBasePosition;
GR::Entity sword_r;


struct SwordAdditionalInfo {
	TVec3 EntityesBasePosition;
	int EntityesFlyOffset;
};


void MouseMove(GR::GrayEngine* Context, double x, double y);
void KeyPress(GR::GrayEngine* Context, GR::EKey key, GR::EAction Action);
void Loop(GR::GrayEngine* Context, double Delta);
TVec2 WindowCenter = TVec2(0);

struct InputHandler {
	std::map<GR::EKey, bool> KeyState;
	TVec2 MouseOffset = TVec2(0.f);
	bool MouseIsLocked = false;
} IH;

int main(int argc, char** argv)
{

	std::string exec_path = "";

	if (argc > 0)
	{
		exec_path = argv[0];
		exec_path = exec_path.substr(0, exec_path.find_last_of('\\') + 1);
	}

	GR::ApplicationSettings Settings = { "Vulkan Application", {1024, 720} };
	std::unique_ptr<GR::GrayEngine> Engine = std::make_unique<GR::GrayEngine>(exec_path, Settings);

	Engine->userPointer1 = &IH;

	Engine->AddInputFunction(Loop);
	Engine->GetEventListener().Subscribe(KeyPress);
	Engine->GetEventListener().Subscribe(MouseMove);
	//registry.emplace_or_replace<GRComponents::Transform>(ent);

	sword = Engine->LoadModel("content\\sword.fbx", nullptr);
	gizmo = Engine->LoadModel("content\\gizmo.fbx", nullptr);

	GRComponents::Transform& wldr = Engine->GetComponent<GRComponents::Transform>(gizmo);
	wldr.SetOffset(TVec3(30.0f, 30.f, 100.f));

	for (int i = 0; i < 50; i++) {
		GR::Entity ent = Engine->LoadModel("content\\cube.fbx", nullptr);
		Engine->EmplaceComponent<SwordAdditionalInfo>(ent);

		GRComponents::Transform& wld = Engine->GetComponent<GRComponents::Transform>(ent);
		GRComponents::Color& clr = Engine->GetComponent<GRComponents::Color>(ent);
		//SwordAdditionalInfo& info = Engine->GetComponent<SwordAdditionalInfo>(ent);

		int radius = 400;
		TVec3 pos = TVec3(
			radius - rand() % (radius * 2),
			radius - rand() % (radius * 2),
			radius - rand() % (radius * 2));
		wld.SetOffset(pos);

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		clr.RGB = TVec3(r, g, b);

		entityes.push_back(ent);
		entityesFlyOffset.push_back(rand());
		entityesBasePosition.push_back(wld.GetOffset());
	}


	Engine->GetMainCamera().View.SetOffset({ 0.0, 0.0, -200.0 });
	WindowCenter = Engine->GetWindow().GetWindowSize() / 2;

	Engine->StartGameLoop();

	return 0;
}

float speed = 500.0f;
float sense = .03f;

void Loop(GR::GrayEngine* Context, double Delta)
{

	TVec3 off = TVec3(0.0);
	TVec2 rot = IH.MouseOffset;

	if (IH.KeyState[GR::EKey::A]) { off.x += Delta * speed; }
	if (IH.KeyState[GR::EKey::D]) { off.x -= Delta * speed; }
	if (IH.KeyState[GR::EKey::W]) { off.z += Delta * speed; }
	if (IH.KeyState[GR::EKey::S]) { off.z -= Delta * speed; }
	if (IH.KeyState[GR::EKey::Key_1]) {
		IH.MouseOffset = TVec2(0.f);
		IH.MouseIsLocked = true;
		Context->GetWindow().DisableCursor(false);
	}
	if (IH.KeyState[GR::EKey::Key_2] ||
		IH.KeyState[GR::EKey::Escape]) {
		IH.MouseIsLocked = false;
		Context->GetWindow().DisableCursor(true);
	}
	if (IH.KeyState[GR::EKey::ArrowLeft]) { rot.y -= 100.f; }
	if (IH.KeyState[GR::EKey::ArrowRight]) { rot.y += 100.f; }
	if (IH.KeyState[GR::EKey::ArrowUp]) { rot.x += 100.f; }
	if (IH.KeyState[GR::EKey::ArrowDown]) { rot.x -= 100.f; }


	if (IH.MouseIsLocked) {
		rot = glm::radians(rot);
		Context->GetMainCamera().View.Rotate(rot.y * sense, -rot.x * sense, 0.);
		TVec3 cameraPos = Context->GetMainCamera().View.GetOffset();

		int d = glm::length2(cameraPos);
		if (d > 10000) {
			//off += glm::normalize(cameraPos);
		}

		//printf("%f %f %f | %d \n", cameraPos.x, cameraPos.y, cameraPos.z, d);

		Context->GetMainCamera().View.Translate(off);

		IH.MouseOffset = TVec2(0.f);
	}


	double angle = Context->GetTime();
	TVec3 cameraPos = Context->GetMainCamera().View.GetOffset() ;
	TMat3 cameraRot = glm::transpose( Context->GetMainCamera().View.GetRotation());
	cameraPos *= -1;

	{
		GRComponents::Transform& wldr = Context->GetComponent<GRComponents::Transform>(gizmo);

		TVec3 newpos = cameraPos - cameraRot * TVec3(30.0f, 30.f, 100.f);

		wldr.SetOffset(newpos);


		TVec3 gpos = wldr.matrix[3];
		TVec3 fwd = glm::normalize(gpos);
		TVec3 rg = glm::normalize(glm::cross(fwd, TVec3(.0f, 1.0f, .0f)));
		TVec3 up = glm::normalize(glm::cross(fwd, rg));
		printf("%6.3f %6.3f %6.3f  \n", gpos.x, gpos.y, gpos.z);
		wldr.SetRotation(rg, up, fwd);

	}

	GRComponents::Transform& wld = Context->GetComponent<GRComponents::Transform>(sword);
	wld.SetOffset(TVec3(.0, glm::sin(angle) * 25.0, 0.0));

	GRComponents::Color& clr = Context->GetComponent<GRComponents::Color>(sword);
	clr.RGB = glm::abs(TVec3(glm::sin(angle), glm::cos(angle), glm::tan(angle)));


	for (int i = 0; i < entityes.size(); i++) {
		int offset = entityesFlyOffset[i];
		TVec3 baseOffset = entityesBasePosition[i];

		GRComponents::Transform& wld = Context->GetComponent<GRComponents::Transform>(entityes[i]);

		TVec3 new_pos = TVec3(.0f, glm::sin(offset + angle) * 25.f, 0.0);
		wld.SetOffset(baseOffset + new_pos);

		//glm::normalize
		TVec3 up = glm::normalize(cameraPos - baseOffset);
		TVec3 rg = glm::cross(up, TVec3(1.f, 1.f, .0f));

		//wld.SetRotation(up, rg);
	}

	Context->GetWindow().SetTitle(("Vulkan Application " + std::format("{:.1f}", 1.0 / Delta)).c_str());
}

void MouseMove(GR::GrayEngine* Context, double x, double y) {
	InputHandler& ih = *static_cast<InputHandler*> (Context->userPointer1);
	WindowCenter = Context->GetWindow().GetWindowSize() / 2;

	if (ih.MouseIsLocked) {
		ih.MouseOffset += TVec2(WindowCenter.x - x, WindowCenter.y - y);
		Context->GetWindow().SetCursorPos(WindowCenter.x, WindowCenter.y);
	}
}




void KeyPress(GR::GrayEngine* Context, GR::EKey key, GR::EAction Action)
{
	InputHandler& ih = *static_cast<InputHandler*> (Context->userPointer1);

	ih.KeyState[key] = Action != GR::EAction::Release;
}