#include "NEngine.hpp"

int main(int argc, char** argv)
{
	SetColor(Blue);
	std::cout << "ENGINE INFO: ";
	ClearColor();
	std::cout << "Welcome to " << PACKAGE_STRING << "! Report bugs to " << PACKAGE_BUGREPORT << "!\n";
	if (!GetGame()->Init(512,512,"Astrostruct"))
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "Failed to initialize the game!\n";
		GetGame()->CleanUp();
		return 1;
	}
	NWindow* MyWindow = GetGame()->GetScene()->AddWindow();
	MyWindow->SetTexture("window");
	MyWindow->SetScale(256,256);
	MyWindow->SetPos(GetGame()->GetWindowSize()/2.f);
	NButton* QuitButton = GetGame()->GetScene()->AddButton();
	QuitButton->SetTexture("button");
	QuitButton->SetParent(MyWindow);
	QuitButton->SetScale(64,32);
	QuitButton->SetText("Quit");
	QuitButton->SetPos(0,-32);
	NButton* PlayButton = GetGame()->GetScene()->AddButton();
	PlayButton->SetTexture("button");
	PlayButton->SetParent(MyWindow);
	PlayButton->SetScale(64,32);
	PlayButton->SetText("Play");
	PlayButton->SetPos(0,32);
	NText* FPSText = GetGame()->GetScene()->AddText("cousine", "FPS: 0");
	FPSText->SetPos(0,FPSText->GetSize()/2.f);
	FPSText->SetColor(0,0,0,1);
	NSound* CoinSound = GetGame()->GetScene()->AddSound("coin");
	while(GetGame()->Running())
	{
		GetGame()->GetInput()->Poll();
		if (PlayButton->OnRelease())
		{
		    CoinSound->Play();
		}
		MyWindow->SetPos(GetGame()->GetWindowSize()/2.f);
		std::stringstream NewText(std::stringstream::in | std::stringstream::out);
		NewText << "FPS: " << 1/GetGame()->GetRender()->GetFrameTime() << '\0';
		FPSText->SetText(NewText.str());
		FPSText->SwapDepth(GetGame()->GetScene()->GetTopDepth());
		GetGame()->GetScene()->Tick();
		GetGame()->GetRender()->Draw();
		GetGame()->Poll();
		if (GetGame()->GetInput()->GetKey(GLFW_KEY_ESC) || QuitButton->OnRelease())
		{
			GetGame()->Close();
		}
	}
	GetGame()->CleanUp();
	return 0;
}
