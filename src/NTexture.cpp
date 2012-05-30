#include "NEngine.hpp"
	
void NAnimation::SetName(std::string i_Name)
{
	Name = i_Name;
}

std::string NAnimation::GetName()
{
	return Name;
}

NAnimation::NAnimation()
{
	Reference = 0;
	FPS = 30;
	Name = "NULL";
}

NAnimation::~NAnimation()
{
}

void NAnimation::AddFrame(std::string FileName)
{
	//Probably need to do some error checking here...
	NCachedTexture* ID = GetGame()->GetRender()->GetCachedTexture(FileName);
	Frames.push_back(ID);
}

NTexture::NTexture(std::string i_Name)
{
	Name = i_Name;
	PlayingAnimation = 0;
	CurrentTime = 0;
	GoodCheck = false;
	IsGood = true;
}

NTexture::NTexture(NTexture* Texture)
{
	Animations = Texture->Animations;
	Name = Texture->Name;
	PlayingAnimation = 0;
	CurrentTime = 0;
	GoodCheck = false;
	IsGood = true;
}

NTexture::~NTexture()
{
}

GLuint NAnimation::GetID(double Time)
{
	unsigned int IDLoc = fmod(Time*FPS,Frames.size());
	return Frames[IDLoc]->ID;
}

bool NAnimation::Good()
{
	for (unsigned int i=0;i<Frames.size();i++)
	{
		if (!Frames[i]->Good())
		{
			return false;
		}
	}
	return true;
}

glm::vec2 NAnimation::GetSize(double Time)
{
	unsigned int IDLoc = fmod(Time*FPS,Frames.size());
	return Frames[IDLoc]->GetSize();
}

void NTexture::Play(std::string i_Name)
{
	for (unsigned int i=0;i<Animations.size();i++)
	{
		if (i_Name == Animations[i]->GetName())
		{
			PlayingAnimation = i;
			return;
		}
	}
	SetColor(Yellow);
	std::cout << "TEXTURE WARN: ";
	ClearColor();
	std::cout << "Animation " << i_Name << " not found in texture " << Name << "\n";
}

bool NTexture::Good()
{
	if (!GoodCheck)
	{
		for (unsigned int i=0;i<Animations.size();i++)
		{
			if (!Animations[i]->Good())
			{
				IsGood = false;
			}
		}
	}
	return IsGood;
}

GLuint NTexture::GetID()
{
	return Animations[PlayingAnimation]->GetID(CurrentTime);
}

void NTexture::AddAnimation(NAnimation* Animation)
{
	Animations.push_back(Animation);
}

void NTexture::Tick(double DT)
{
	CurrentTime += DT;
}

glm::vec2 NTexture::GetSize()
{
	return Animations[PlayingAnimation]->GetSize(CurrentTime);
}

float NTexture::GetFloat(std::string i_Name)
{
	return Animations[PlayingAnimation]->GetFloat(i_Name);
}

float NAnimation::GetFloat(std::string i_Name)
{
	lua_State* L = GetGame()->GetLua()->GetL();
	lua_getref(L,Reference);
	lua_getfield(L,-1,i_Name.c_str());
	if (!lua_isnumber(L,-1))
	{
		SetColor(Yellow);
		std::cout << "LUA WARN: ";
		ClearColor();
		std::cout << "Tried to use variable " << i_Name << " as a number (It's not a number or doesn't exist!).\n";
		lua_pop(L,2);
		return 0;
	}
	float Number = lua_tonumber(L,-1);
	lua_pop(L,2);
	return Number;
}

