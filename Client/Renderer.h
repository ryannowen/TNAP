#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <limits>

struct Entity
{
	Entity()
	{}
	Entity(const std::string& argName, const std::vector<std::shared_ptr<Entity>>& argChildren)
		:	name(argName), children(argChildren)
	{}
	std::string name;
	std::vector<std::shared_ptr<Entity>> children;
	static int treeIndex;
	static Entity* selected;

	void ImGuiDrawTree()
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this == selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		if(0 == children.size())
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)treeIndex, node_flags, name.c_str());
		if (ImGui::IsItemClicked())
			selected = this;



		treeIndex++;

		if (node_open)
		{
			for (int i = 0; i < children.size(); i++)
			{
				children[i]->ImGuiDrawTree();
			}
			if (0 != children.size())
				ImGui::TreePop();
		}
	}

	virtual void ImGuiDrawInformation()
	{
		ImGui::Text(name.c_str());
	}
};

struct Player : public Entity
{
	Player(const std::string& argName, const std::vector<std::shared_ptr<Entity>>& argChildren)
		:	Entity(argName, argChildren)
	{}

	int movementSpeed = 1;
	int damage = 0;

	float colour[4]{ 0, 0, 0, 0 };

	virtual void ImGuiDrawInformation() override
	{
		ImGui::Text(name.c_str());
		ImGui::SliderInt("MovementSpeed", &movementSpeed, 0, 100);
		ImGui::InputInt("Damage", &damage);
		ImGui::ColorEdit4("Player Colour", colour);
	}
};

struct MeshTest : public Entity
{
	MeshTest(const std::string& argName, const std::vector<std::shared_ptr<Entity>>& argChildren)
		: Entity(argName, argChildren)
	{}

	glm::vec3 translation{ 0 };
	glm::vec3 rotation{ 0 };
	glm::vec3 scale{ 1 };

	float emission{ 50 };
	float metalltic{ 50 };
	float roughness{ 50 };

	int matCount = 1;

	GLuint text1{ 0 };
	GLuint text2{ 0 };
	GLuint text3{ 0 };

	virtual void ImGuiDrawInformation() override
	{
		ImGui::Text(name.c_str());
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::InputFloat3("Translation", &translation.x);
			ImGui::InputFloat3("Rotation", &rotation.x);
			ImGui::InputFloat3("Scale", &scale.x);
		}
		if (ImGui::CollapsingHeader("Materials"))
		{
			ImGui::InputInt("Material Count", &matCount);

			if (matCount <= 0)
				matCount = 1;

			for (int i = 0; i < matCount; i++)
			{
				if (ImGui::CollapsingHeader(std::string("Material_").append(std::to_string(i)).c_str()))
				{
					ImGui::ImageButton((ImTextureID)text1, ImVec2(64, 64));
					if (ImGui::IsItemClicked())
					{
						text1 = 0;
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
						{
							text1 = *(const GLuint*)payload->Data;
						}

						ImGui::EndDragDropTarget();
					}					
					ImGui::SameLine();
					ImGui::SliderFloat("Emission", &emission, 0, 100);

					ImGui::ImageButton((ImTextureID)text2, ImVec2(64, 64));
					if (ImGui::IsItemClicked())
					{
						text2 = 0;
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
						{
							text2 = *(const GLuint*)payload->Data;
						}

						ImGui::EndDragDropTarget();
					}	
					ImGui::SameLine();
					ImGui::SliderFloat("Metalltic", &metalltic, 0, 100);

					ImGui::ImageButton((ImTextureID)text3, ImVec2(64, 64));
					if (ImGui::IsItemClicked())
					{
						text3 = 0;
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
						{
							text3 = *(const GLuint*)payload->Data;
						}

						ImGui::EndDragDropTarget();
					}	
					ImGui::SameLine();
					ImGui::SliderFloat("Roughness", &roughness, 0, 100);
				}
			}
		}
	}
};

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };
	GLuint m_VAO2{ 0 };

	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };
	GLuint m_numElements2{ 0 };

	bool CreateProgram();

	GLuint fbo;
	GLuint textureBuffer;
	GLuint m_DepthAttachment;

	ImVec2 windowSize;

	std::vector<GLuint> contextTextures;
	std::vector<std::string> textureNames
	{
		"aqua_pig_2K.png",
		"MissingTexture.jpg",
		"Grass.jpg",
		"jeep_army.jpg",
		"rubik.png"
	};

	GLuint matTexture{ 0 };

	std::vector<std::shared_ptr<Entity>> entities
	{
		std::make_shared<Player>("Player", std::vector<std::shared_ptr<Entity>>()),
		std::make_shared<MeshTest>("Mesh", std::vector<std::shared_ptr<Entity>>
			{
			std::make_shared<Entity>("Entity 3", std::vector<std::shared_ptr<Entity>>()),
			std::make_shared<Entity>("Entity 4", std::vector<std::shared_ptr<Entity>>())
			}),
		std::make_shared<Entity>("Entity 5", std::vector<std::shared_ptr<Entity>>()),
		std::make_shared<Entity>("Entity 6", std::vector<std::shared_ptr<Entity>>
			{
				std::make_shared<Entity>("Entity 7", std::vector<std::shared_ptr<Entity>>()),
				std::make_shared<Entity>("Entity 8", std::vector<std::shared_ptr<Entity>>
					{
					std::make_shared<Entity>("Entity 9", std::vector<std::shared_ptr<Entity>>
						{
						std::make_shared<Entity>("Entity 10", std::vector<std::shared_ptr<Entity>>
							{
							std::make_shared<Entity>("Entity 11", std::vector<std::shared_ptr<Entity>>
								{
								std::make_shared<Entity>("Entity 12", std::vector<std::shared_ptr<Entity>>())
								})
							})
						})
					})
			}),
	};

public:
	Renderer()=default;
	~Renderer();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);

	bool viewportSelected{ false };
};

