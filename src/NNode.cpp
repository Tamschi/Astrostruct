#include "NEngine.hpp"

NNode::NNode(NodeType i_Type)
{
    ID = 0;
    Position = glm::vec3(0);
    Scale = glm::vec3(1);
    Color = glm::vec4(1);
    Angle = glm::vec3(0);
    Parent = NULL;
    Matrix = glm::mat4();
    SMatrix = glm::mat4();
    Type = i_Type;
    GetGame()->GetScene()->AddNode(this);
}

NNode::~NNode()
{
    while(Children.size())
    {
        Children[0]->SetParent(NULL);
        GetGame()->GetScene()->Remove(Children[0]);
        Children.erase(Children.begin());
    }
    if (Parent)
    {
        Parent->RemoveChild(this,true);
    }
}

void NNode::SetID(unsigned int i_ID)
{
    ID = i_ID;
}
unsigned int NNode::GetID()
{
    return ID;
}

NNode* NNode::GetParent()
{
    return Parent;
}

void NNode::SetPos(glm::vec3 i_Position)
{
    Position = i_Position;
    UpdateMatrix();
}

void NNode::SetAng(glm::vec3 i_Angle)
{
    Angle = i_Angle;
    UpdateMatrix();
}

void NNode::SetAng(float i_Angle)
{
    SetAng(glm::vec3(0,0,i_Angle));
}

void NNode::SetParent(NNode* Node)
{
    if (Parent != NULL && Node != NULL)
    {
        Parent->RemoveChild(this);
    }
    Parent = Node;
    if (Node != NULL)
    {
        Parent->AddChild(this);
        UpdateMatrix();
    }
}

void NNode::AddChild(NNode* Node)
{
    Children.push_back(Node);
    UpdateMatrix();
}

void NNode::RemoveChild(NNode* Node, bool Removing)
{
    for (unsigned int i=0;i<Children.size();i++)
    {
        if (Children[i] == Node)
        {
            if (!Removing)
            {
                Children[i]->SetParent(NULL);
                Children[i]->UpdateMatrix();
            }
            Children.erase(Children.begin()+i);
            return;
        }
    }
}

glm::vec3 NNode::GetAng()
{
    return Angle;
}

glm::vec3 NNode::GetRealPos()
{
    if (Parent)
    {
        return Parent->GetRealPos()+Position;
    }
    return Position;
}

glm::vec3 NNode::GetPos()
{
    return Position;
}

glm::mat4 NNode::GetModelMatrix()
{
    return Matrix;
}

void NNode::UpdateMatrix()
{
    glm::mat4 Pos = glm::translate(glm::mat4(),Position);
    glm::mat4 Ang = glm::rotate(glm::mat4(),Angle.z,glm::vec3(0,0,1));
    Ang = glm::rotate(Ang,Angle.y,glm::vec3(0,1,0));
    Ang = glm::rotate(Ang,Angle.x,glm::vec3(1,0,0));
    glm::mat4 Sca = glm::scale(glm::mat4(),Scale);
    if (Parent == NULL)
    {
        SMatrix = Pos*Ang;
        Matrix = Pos*Ang*Sca;
    } else {
        SMatrix = Parent->GetNonScaleMatrix()*(Pos*Ang);
        Matrix = Parent->GetNonScaleMatrix()*(Pos*Ang*Sca);
    }
    for (unsigned int i=0;i<Children.size();i++)
    {
        Children[i]->UpdateMatrix();
    }
}

NNode* NNode::GetRoot()
{
    if (Parent == NULL)
    {
        return this;
    }
    return Parent->GetRoot();
}

glm::mat4 NNode::GetNonScaleMatrix() {
    return SMatrix;
}

glm::vec3 NNode::GetScale()
{
    return Scale;
}

void NNode::SetScale(float W, float H)
{
    SetScale(glm::vec3(W,H,GetScale().z));
}

void NNode::SetScale(glm::vec3 i_Scale)
{
    Scale = i_Scale;
    UpdateMatrix();
}

void NNode::SetScale(glm::vec2 i_Scale)
{
    Scale = glm::vec3(i_Scale,GetScale().z);
}

void NNode::SetScale(float i_Scale)
{
    SetScale(glm::vec3(i_Scale));
}

void NNode::Tick(double DT) {}
void NNode::Draw(NCamera* View) {}

void NNode::SetColor(glm::vec4 i_Color)
{
    Color = i_Color;
}
void NNode::SetColor(float R, float G, float B, float A)
{
    SetColor(glm::vec4(R,G,B,A));
}
void NNode::SetColor(float R, float G, float B)
{
    float A = Color.w;
    SetColor(glm::vec4(R,G,B,A));
}
void NNode::SetColor(glm::vec3 i_Color)
{
    float A = Color.w;
    SetColor(glm::vec4(i_Color,A));
}
glm::vec4 NNode::GetColor()
{
    return Color;
}
void NNode::Remove()
{
    GetGame()->GetScene()->Remove(this);
}
NodeType NNode::GetType()
{
    return Type;
}
void NNode::SetLayer(unsigned int Layer)
{
    GetGame()->GetScene()->SwapLayer(this,Layer);
}
void NNode::Unallocate()
{
    delete this;
}
