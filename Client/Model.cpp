#include "Model.hpp"

namespace TNAP {

	Model::Model()
	{
	}

	bool Model::loadFromFile(const std::string& argFilePath)
	{
		m_filepath = argFilePath;

		// Log loading file

		// Commom post processing steps - may slow load but make mesh better optimised
		unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
			aiProcess_JoinIdenticalVertices |				// join identical vertices/ optimize indexing
			aiProcess_ValidateDataStructure |				// perform a full validation of the loader's output
			aiProcess_ImproveCacheLocality |				// improve the cache locality of the output vertices
			aiProcess_RemoveRedundantMaterials |            // remove redundant materials
			aiProcess_FindDegenerates |						// remove degenerated polygons from the import
			aiProcess_FindInvalidData |						// detect invalid model data, such as invalid normal vectors
			aiProcess_GenUVCoords |							// convert spherical, cylindrical, box and planar mapping to proper UVs
			aiProcess_TransformUVCoords |					// preprocess UV transformations (scaling, translation ...)
			aiProcess_FindInstances |						// search for instanced meshes and remove them by references to one master
			aiProcess_LimitBoneWeights |					// limit bone weights to 4 per vertex
			aiProcess_OptimizeMeshes |						// join small meshes, if possible;
			aiProcess_SplitByBoneCount |					// split meshes with too many bones.
			aiProcess_GenSmoothNormals |					// generate smooth normal vectors if not existing
			aiProcess_SplitLargeMeshes |					// split large, unrenderable meshes into submeshes
			aiProcess_Triangulate |							// triangulate polygons with more than 3 edges
			aiProcess_SortByPType |							// make 'clean' meshes which consist of a single typ of primitives
			aiProcess_GenSmoothNormals |					// if no normals then create them
			0;

		// Create an instance of the Importer class
		Assimp::Importer importer;

		// By removing all points and lines we guarantee a face will describe a 3 vertex triangle
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
		importer.SetPropertyInteger(AI_CONFIG_GLOB_MEASURE_TIME, 1);

		const aiScene* scene = importer.ReadFile(argFilePath.c_str(), ppsteps);

		if (!scene)
		{
			//EsOutput(importer.GetErrorString());
			return false;
		}

		populateModelData(scene);

		// Might need to remove later
		for (std::unique_ptr<Helpers::Mesh>& mesh : m_meshes)
		{
			bindMesh(mesh.get());

			bool isUnique = true;
			for (const size_t matIndex : m_uniqueMaterialIndices)
			{
				if (matIndex == mesh->materialIndex)
				{
					isUnique = false;
					break;
				}
			}
			if (isUnique)
			{
				m_uniqueMaterialIndices.push_back(mesh->materialIndex);
			}
		}

		m_defaultMaterialHandles.reserve(m_uniqueMaterialIndices.size());

		return true;
	}

	void Model::populateModelData(const aiScene* const argScene)
	{
		/*if (scene->HasCameras())
			// Do something
		if (scene->HasLights())
			// Do something

		if (!scene->HasMeshes())
		{
			// Log No meshes
			return;
		}

		// Materials are held scene wide and referenced in the part by id so need to grab here
		m_materials.resize(scene->mNumMaterials);
		for (unsigned int m = 0; m < scene->mNumMaterials; m++)
		{
			// Materials are held against property keys
			aiColor4D col;
			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_AMBIENT, col))
				m_materials[m].ambientColour = aiColor4DToGlmVec4(col);

			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_DIFFUSE, col))
				m_materials[m].diffuseColour = aiColor4DToGlmVec4(col);

			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_SPECULAR, col))
				m_materials[m].specularColour = aiColor4DToGlmVec4(col);

			if (AI_SUCCESS == scene->mMaterials[m]->Get(AI_MATKEY_COLOR_EMISSIVE, col))
				m_materials[m].emissiveColour = aiColor4DToGlmVec4(col);

			unsigned int shininess = 0;
			unsigned int ret1 = scene->mMaterials[m]->Get(AI_MATKEY_SHININESS, shininess);
			unsigned int shininessStrength = 0;
			unsigned int ret2 = scene->mMaterials[m]->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			float specularFactor = 0;
			if (ret1 == AI_SUCCESS && ret2 == AI_SUCCESS)
				m_materials[m].specularFactor = (float)(shininess * shininessStrength);
			else if (ret1 == AI_SUCCESS)
				m_materials[m].specularFactor = (float)shininess; // TODO: not sure about this		

			// There are many types for each colour and also normals
			aiString texPath;
			if (AI_SUCCESS == scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
			{
				// Just storing the filename for now, the factory will have to create a texture id
				m_materials[m].diffuseTextureFilename = std::string(texPath.C_Str());
			}

			if (AI_SUCCESS == scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, 0, &texPath))
			{
				// Just storing the filename for now, the factory will have to create a texture id
				m_materials[m].specularTextureFilename = std::string(texPath.C_Str());
			}

			// Ignoring others - report
			for (int i = aiTextureType_AMBIENT; i < aiTextureType_UNKNOWN; i++)
			{
				if (AI_SUCCESS == scene->mMaterials[m]->GetTexture((aiTextureType)i, 0, &texPath))
					EsOutput("Ignoring: material texture type: " + std::to_string(i));
			}
		}*/

		int hasBones{ 0 };
		int hasTangents{ 0 };
		int hasColourChannels{ 0 };
		int hasMMoreThanOneUVChannel{ 0 };

		//EsOutput("Scene contains " + std::to_string(scene->mNumMeshes) + " mesh");

		// ASSIMP mesh
		// http://assimp.sourceforge.net/lib_html/structai_mesh.html
		for (unsigned int i = 0; i < argScene->mNumMeshes; i++)
		{
			aiMesh* const aimesh = argScene->mMeshes[i];

			if (aimesh->HasBones())
				hasBones++;
			if (aimesh->GetNumColorChannels())
				hasColourChannels++;
			if (aimesh->GetNumUVChannels() > 1)
				hasMMoreThanOneUVChannel++;
			if (aimesh->HasTangentsAndBitangents())
				hasTangents++;

			// Create my mesh part
			m_meshes.push_back(std::make_unique<Helpers::Mesh>());
			Helpers::Mesh* newMesh = m_meshes.back().get();

			newMesh->name = aimesh->mName.C_Str();

			//		EsOutput("Processing mesh with name: " + newMesh->name);

					// Copy over all the vertices, ai format of a vertex is same as mine
					// Probably be able to speed this all up at some point
			for (size_t v = 0; v < aimesh->mNumVertices; v++)
			{
				glm::vec3 newV = *(glm::vec3*) & aimesh->mVertices[v];
				newMesh->vertices.push_back(newV);
			}

			// And the normals if there are any
			if (aimesh->HasNormals())
			{
				for (size_t v = 0; v < aimesh->mNumVertices; v++)
				{
					glm::vec3 newN = *(glm::vec3*) & aimesh->mNormals[v];
					newMesh->normals.push_back(newN);
				}
			}

			// And texture coordinates
			if (aimesh->HasTextureCoords(0))
			{
				for (size_t v = 0; v < aimesh->mNumVertices; v++)
				{
					glm::vec2 newN = *(glm::vec2*) & aimesh->mTextureCoords[0][v];
					newMesh->uvCoords.push_back(newN);
				}
			}

			// Faces contain the vertex indices and due to the flags I set before are always triangles
			for (unsigned int face = 0; face < aimesh->mNumFaces; face++)
			{
				assert(aimesh->mFaces[face].mNumIndices == 3);
				for (int triInd = 0; triInd < 3; triInd++)
					newMesh->elements.push_back(aimesh->mFaces[face].mIndices[triInd]);
			}

			// Material index
			newMesh->materialIndex = aimesh->mMaterialIndex;
		}

		/*if (hasBones)
			EsOutput("Ignoring: One or more mesh have bones");
		if (hasColourChannels)
			EsOutput("Ignoring: One or more mesh has colour channels");
		if (hasMMoreThanOneUVChannel)
			EsOutput("Ignoring: One or more mesh has more than one UV channel");
		if (hasTangents)
			EsOutput("Ignoring: One or more mesh has tangents");

		// Hierarchy, ASSIMP calls these nodes
		m_rootNode = RecurseCreateNode(scene->mRootNode, nullptr);

		for (size_t i = 0; i < scene->mNumAnimations; i++)
		{
			// Only supporting node animation			
			if (scene->mAnimations[i]->mNumMeshChannels)
				EsOutput("Ignoring: mesh animations");

			if (scene->mAnimations[i]->mNumChannels)
				EsOutput("Animation has " + std::to_string(scene->mAnimations[i]->mNumChannels) + " Channels");
		}*/
	}

	void Model::bindMesh(Helpers::Mesh* const argMesh)
	{
		/// Reference to Objects
		GLuint VAO, verticesVBO, uvVBO, normalsVBO, elementsEBO;

		/// Generates VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		/// Generate objects and Bind them with data
		glGenBuffers(1, &verticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * argMesh->vertices.size(), argMesh->vertices.data(), GL_STATIC_DRAW);

		/// Sends Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * argMesh->normals.size(), argMesh->normals.data(), GL_STATIC_DRAW);

		/// Sends Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * argMesh->uvCoords.size(), argMesh->uvCoords.data(), GL_STATIC_DRAW);

		/// Sends uvs
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		/// Generates Element Buffer and Binds it
		glGenBuffers(1, &elementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * argMesh->elements.size(), argMesh->elements.data(), GL_STATIC_DRAW);

		/// Binds Elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);

		/// Clear bindings
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		argMesh->VAO = VAO;

		// Good idea to check for an error now:	
		Helpers::CheckForGLError();

		// Clear VAO binding
		glBindVertexArray(0);
	}

	void Model::getLocalExtents(glm::vec3& argMinExtents, glm::vec3& argMaxExtents) const
	{
		if (m_meshes.empty())
			return;

		m_meshes[0]->GetLocalExtents(argMinExtents, argMaxExtents);

		for (size_t i = 1; i < m_meshes.size(); i++)
		{
			glm::vec3 newMin;
			glm::vec3 newMax;
			m_meshes[0]->GetLocalExtents(newMin, newMax);

			argMinExtents.x = std::min(argMinExtents.x, newMin.x);
			argMinExtents.y = std::min(argMinExtents.y, newMin.y);
			argMinExtents.z = std::min(argMinExtents.z, newMin.z);

			argMaxExtents.x = std::max(argMaxExtents.x, newMax.x);
			argMaxExtents.y = std::max(argMaxExtents.y, newMax.y);
			argMaxExtents.z = std::max(argMaxExtents.z, newMax.z);
		}
	}
}