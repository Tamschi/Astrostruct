#include "NEngine.hpp"

NLight::NLight(std::string i_Texture)
{
	Changed = true;
	SChanged = true;
	Texture = GetGame()->GetRender()->GetTexture(i_Texture);
	glGenBuffers(3,Buffers);
	Shader = GetGame()->GetRender()->GetShader("flat");
	if (Shader != NULL)
	{
		MatrixLoc = Shader->GetUniformLocation("MVP");
		TextureLoc = Shader->GetUniformLocation("Texture");
		ColorLoc = Shader->GetUniformLocation("Color");
	}
	ShadowShader = GetGame()->GetRender()->GetShader("normal_textureless");
	if (ShadowShader != NULL)
	{
		SMatrixLoc = Shader->GetUniformLocation("MVP");
		SColorLoc = Shader->GetUniformLocation("Color");
	}
}

NLight::~NLight()
{
	if (Texture)
	{
		delete Texture;
	}
	glDeleteBuffers(3,Buffers);
}

void NLight::GenerateLightBuffers()
{
	if (!Texture)
	{
		return;
	}
	if (!Changed)
	{
		return;
	}
	Verts.clear();
	UVs.clear();
	Verts.push_back(glm::vec2(-.5,-.5));
	UVs.push_back(glm::vec2(0,1));
	Verts.push_back(glm::vec2(.5,-.5));
	UVs.push_back(glm::vec2(1,1));
	Verts.push_back(glm::vec2(.5,.5));
	UVs.push_back(glm::vec2(1,0));
	Verts.push_back(glm::vec2(-.5,.5));
	UVs.push_back(glm::vec2(0,0));
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
	Changed = false;
}

void NLight::GenerateShadowBuffers()
{
	if (!Texture)
	{
		return;
	}
	//if (!SChanged)
	//{
	//	return;
	//}
	Shadows.clear();
	for (float x = -GetScale().x;x<GetScale().x;x+=GetGame()->GetMap()->GetTileSize())
	{
		for (float y = -GetScale().y;y<GetScale().y;y+=GetGame()->GetMap()->GetTileSize())
		{
			NTile* Tile = GetGame()->GetMap()->GetTile(GetPos()+glm::vec3(x,y,0));
			if (Tile == NULL || !Tile->IsOpaque())
			{
				continue;
			}
			float TS = GetGame()->GetMap()->GetTileSize()/2.f;
			glm::vec3 TPos = GetGame()->GetMap()->TilePos(GetPos()+glm::vec3(x,y,0));
			glm::vec3* Points;
			Points = new glm::vec3[4];
			Points[0] = TPos+glm::vec3(TS,TS,0);
			Points[1] = TPos+glm::vec3(TS,-TS,0);
			Points[2] = TPos+glm::vec3(-TS,-TS,0);
			Points[3] = TPos+glm::vec3(-TS,TS,0);
			std::vector<glm::vec4> Faces;
			//Generate faces
			NTile* CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y-1,Tile->Z);
			if (CheckTile && !CheckTile->IsOpaque())
			{
				Faces.push_back(glm::vec4(Points[1].x,Points[1].y,Points[2].x,Points[2].y));
			}
			CheckTile = GetGame()->GetMap()->GetTile(Tile->X,Tile->Y+1,Tile->Z);
			if (CheckTile && !CheckTile->IsOpaque())
			{
				Faces.push_back(glm::vec4(Points[3].x,Points[3].y,Points[0].x,Points[0].y));
			}
			CheckTile = GetGame()->GetMap()->GetTile(Tile->X-1,Tile->Y,Tile->Z);
			if (CheckTile && !CheckTile->IsOpaque())
			{
				//Faces.push_back(glm::vec4(Points[3].x,Points[3].y,Points[2].x,Points[2].y));
				Faces.push_back(glm::vec4(Points[2].x,Points[2].y,Points[3].x,Points[3].y));
			}
			CheckTile = GetGame()->GetMap()->GetTile(Tile->X+1,Tile->Y,Tile->Z);
			if (CheckTile && !CheckTile->IsOpaque())
			{
				Faces.push_back(glm::vec4(Points[0].x,Points[0].y,Points[1].x,Points[1].y));
			}
			for (unsigned int i=0;i<Faces.size();i++)
			{
				//Remove front faces
				if (!Facing(glm::vec2(GetPos().x,GetPos().y),Faces[i]))
				{
					continue;
				}
				//Generate shadow mesh
				float Radians = -atan2(GetPos().x-Faces[i].x,GetPos().y-Faces[i].y)-PI/2.f;
				float BRadians = -atan2(GetPos().x-Faces[i].z,GetPos().y-Faces[i].w)-PI/2.f;
				Shadows.push_back(glm::vec3(Faces[i].x,Faces[i].y,TPos.z));
				Shadows.push_back(glm::vec3(Faces[i].z,Faces[i].w,TPos.z));
				Shadows.push_back(glm::vec3(Faces[i].z+cos(BRadians)*GetScale().x,Faces[i].w+sin(BRadians)*GetScale().y,TPos.z));
				Shadows.push_back(glm::vec3(Faces[i].x+cos(Radians)*GetScale().x,Faces[i].y+sin(Radians)*GetScale().y,TPos.z));
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
	glBufferData(GL_ARRAY_BUFFER,Shadows.size()*sizeof(glm::vec3),&Shadows[0],GL_STATIC_DRAW);
	SChanged = false;
}

void NLight::Draw(NCamera* View)
{
	if (GetGame()->GetMap()->GetLevel() != GetGame()->GetMap()->GetLevel(GetPos()))
	{
		return;
	}
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS,0x1,0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	DrawShadow(View);
	glStencilFunc(GL_EQUAL,0x0,0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	DrawLight(View);
	glDisable(GL_STENCIL_TEST);
}
void NLight::DrawLight(NCamera* View)
{
	GenerateLightBuffers();
	if (Texture == NULL || GetColor().w == 0)
	{
		return;
	}
	if (Shader == NULL)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
		glVertexPointer(2,GL_FLOAT,0,NULL);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
		glTexCoordPointer(2,GL_FLOAT,0,NULL);

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (Texture != NULL)
		{
			glBindTexture(GL_TEXTURE_2D,Texture->GetID());
		}

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&View->GetPerspMatrix()[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glm::mat4 MVP = View->GetPerspViewMatrix()*GetModelMatrix();
		glLoadMatrixf(&MVP[0][0]);

		glColor4fv(&(GetColor()[0]));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_QUADS,0,Verts.size());
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		return;
	}
	glUseProgram(Shader->GetID());
	glActiveTexture(GL_TEXTURE0);
	if (Texture != NULL)
	{
		glBindTexture(GL_TEXTURE_2D,Texture->GetID());
	}
	glUniform1i(TextureLoc,0);
	glm::mat4 MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
	glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
	glUniform4fv(ColorLoc,1,&(GetColor()[0]));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glDrawArrays(GL_QUADS,0,Verts.size());
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
}
void NLight::DrawShadow(NCamera* View)
{
	GenerateShadowBuffers();
	if (Texture == NULL || GetColor().w == 0)
	{
		return;
	}
	if (ShadowShader == NULL)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
		glVertexPointer(2,GL_FLOAT,0,NULL);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&View->GetPerspMatrix()[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glm::mat4 MVP = View->GetViewMatrix();
		glLoadMatrixf(&MVP[0][0]);

		glColor4f(1,1,1,1);
		glDrawArrays(GL_QUADS,0,Verts.size());

		glDisableClientState(GL_VERTEX_ARRAY);
		return;
	}
	glUseProgram(ShadowShader->GetID());
	glm::mat4 MVP = View->GetPerspMatrix()*View->GetViewMatrix();
	glUniformMatrix4fv(SMatrixLoc,1,GL_FALSE,&MVP[0][0]);
	glUniform4f(SColorLoc,0,0,0,0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS,0,Shadows.size());
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glUseProgram(0);
}

NLightSystem::NLightSystem()
{
	glGenFramebuffers(1,&FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	//Texture
	glGenTextures(1,&FrameBufferTexture);
	glBindTexture(GL_TEXTURE_2D,FrameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //According to the tutorial I'm following, frame buffers require bad filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,FrameBufferTexture,0);

	//Stencil
	/*glGenTextures(1,&StencilBuffer);
	glBindTexture(GL_TEXTURE_2D,StencilBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //According to the tutorial I'm following, frame buffers require bad filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,StencilBuffer,0);*/
	glGenRenderbuffers(1,&StencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,StencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_STENCIL,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,StencilBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,StencilBuffer);

	GetGame()->GetRender()->CheckFramebuffer();
	glClearColor(1,1,1,1);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//Generate screen quad
	Shader = GetGame()->GetRender()->GetShader("flat_colorless");
	if (Shader != NULL)
	{
		TextureLoc = Shader->GetUniformLocation("Texture");
	}
	glGenBuffers(2,VertexBuffers);
	Verts.clear();
	UVs.clear();
	Verts.push_back(glm::vec2(-1,-1));
	UVs.push_back(glm::vec2(0,0));
	Verts.push_back(glm::vec2(1,-1));
	UVs.push_back(glm::vec2(1,0));
	Verts.push_back(glm::vec2(1,1));
	UVs.push_back(glm::vec2(1,1));
	Verts.push_back(glm::vec2(-1,1));
	UVs.push_back(glm::vec2(0,1));
	glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
}

void NLightSystem::CheckFBO()
{
	if (!GetGame()->GetWindowChanged())
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	glBindTexture(GL_TEXTURE_2D,FrameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //According to the tutorial I'm following, frame buffers require bad filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,FrameBufferTexture,0);
	glBindRenderbuffer(GL_RENDERBUFFER,StencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_STENCIL,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,StencilBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,StencilBuffer);
	GetGame()->GetRender()->CheckFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

NLightSystem::~NLightSystem()
{
	glDeleteFramebuffers(1,&FrameBuffer);
	glDeleteTextures(1,&FrameBufferTexture);
	glDeleteRenderbuffers(1,&StencilBuffer);
	glDeleteBuffers(2,VertexBuffers);
}
GLuint NLightSystem::GetFramebuffer()
{
	return FrameBuffer;
}

void NLightSystem::Draw()
{
	CheckFBO();
	glEnable(GL_TEXTURE_2D);
	glUseProgram(Shader->GetID());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,FrameBufferTexture);
	glUniform1i(TextureLoc,0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	glEnable(GL_TEXTURE_2D);
	glDrawArrays(GL_QUADS,0,Verts.size());
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
}

std::string NLight::Type()
{
	return "Light";
}

void NLight::Remove()
{
	delete (NLight*)this;
}

void NLight::Tick(double DT)
{
}