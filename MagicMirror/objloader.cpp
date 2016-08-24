#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <tuple>
#include <algorithm>
#include <unordered_map>

#include <glm/glm.hpp>

#include "objloader.hpp"

typedef std::tuple<int, int, int> point_index;

point_index make_point_index(int v0, int uv0 = -1, int n0 = -1) {
	return std::make_tuple(v0, uv0, n0);
}

struct key_hash : public std::unary_function<point_index, std::size_t>
{
	std::size_t operator()(const point_index& k) const
	{
		return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
	}
};

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	std::vector<std::tuple<int, int, int>> & out_faces
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::unordered_map<point_index, int, key_hash> point_index_set;

	out_faces.clear();

	FILE * file;
	fopen_s(&file, path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			std::vector<unsigned int> vertexIndex(3), uvIndex(3), normalIndex(3);
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches == 1){

				int matches = fscanf(file, "%d %d\n", &vertexIndex[1], &vertexIndex[2]);
				std::sort(vertexIndex.begin(), vertexIndex.end());

				int v0, v1, v2;
				point_index pi0 = make_point_index(vertexIndex[0] - 1);
				point_index pi1 = make_point_index(vertexIndex[1] - 1);
				point_index pi2 = make_point_index(vertexIndex[2] - 1);

				std::pair<std::unordered_map<point_index, int, key_hash>::iterator, bool> insert_result = point_index_set.insert(std::pair<point_index, int>(pi0, point_index_set.size()));
				v0 = (*(insert_result.first)).second;																							   
				insert_result = point_index_set.insert(std::pair<point_index, int>(pi1, point_index_set.size()));
				v1 = (*(insert_result.first)).second;																							   
				insert_result = point_index_set.insert(std::pair<point_index, int>(pi2, point_index_set.size()));
				v2 = (*(insert_result.first)).second;

				out_faces.push_back(std::make_tuple(v0, v1, v2));
			}
			else if (matches == 9){
				std::sort(vertexIndex.begin(), vertexIndex.end());
				std::sort(uvIndex.begin(), uvIndex.end());
				std::sort(normalIndex.begin(), normalIndex.end());

				int v0, v1, v2;
				point_index pi0 = make_point_index(vertexIndex[0] - 1, uvIndex[0] - 1, normalIndex[0] - 1);
				point_index pi1 = make_point_index(vertexIndex[1] - 1, uvIndex[1] - 1, normalIndex[1] - 1);
				point_index pi2 = make_point_index(vertexIndex[2] - 1, uvIndex[2] - 1, normalIndex[2] - 1);

				std::pair<std::unordered_map<point_index, int, key_hash>::iterator, bool> insert_result = point_index_set.insert(std::pair<point_index, int>(pi0, point_index_set.size()));
				v0 = (*(insert_result.first)).second;
				insert_result = point_index_set.insert(std::pair<point_index, int>(pi1, point_index_set.size()));
				v1 = (*(insert_result.first)).second;
				insert_result = point_index_set.insert(std::pair<point_index, int>(pi2, point_index_set.size()));
				v2 = (*(insert_result.first)).second;

				out_faces.push_back(std::make_tuple(v0, v1, v2));
			}
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// split vertices/UVs/normals accdg to faces
	out_vertices.resize(point_index_set.size());
	out_uvs.resize(point_index_set.size());
	out_normals.resize(point_index_set.size());

	bool full_obj = true;

	for (std::pair<point_index, int> pi : point_index_set) {
		int v, uv, n, i;
		std::tie(v, uv, n) = pi.first;
		i = pi.second;
		out_vertices[i] = (temp_vertices[v]);

		if (full_obj) {
			if (uv >= 0 && n >= 0) {
				out_uvs[i] = (temp_uvs[uv]);
				out_normals[i] = (temp_normals[n]);
			}
			else {
				full_obj = false;
			}
		}
	}

	fclose(file);

	return true;
}


bool loadOBJsimple(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<std::tuple<int, int, int>> & out_faces
	) {
	printf("Loading OBJ file %s...\n", path);

	FILE * file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		return false;
	}

	out_vertices.clear();
	out_faces.clear();

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			out_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			std::vector<unsigned int> vertexIndex(3), uvIndex(3), normalIndex(3);

			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			std::sort(vertexIndex.begin(), vertexIndex.end());

			out_faces.push_back(std::make_tuple(vertexIndex[0] - 1, vertexIndex[1] - 1, vertexIndex[2] - 1));
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	fclose(file);

	return true;
}


#ifdef USE_ASSIMP // don't use this #define, it's only for me (it AssImp fails to compile on your machine, at least all the other tutorials still work)

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

bool loadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
){

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
	if( !scene) {
		fprintf( stderr, importer.GetErrorString());
		getchar();
		return false;
	}
	const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

	// Fill vertices positions
	vertices.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D pos = mesh->mVertices[i];
		vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
	}

	// Fill vertices texture coordinates
	uvs.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
		uvs.push_back(glm::vec2(UVW.x, UVW.y));
	}

	// Fill vertices normals
	normals.reserve(mesh->mNumVertices);
	for(unsigned int i=0; i<mesh->mNumVertices; i++){
		aiVector3D n = mesh->mNormals[i];
		normals.push_back(glm::vec3(n.x, n.y, n.z));
	}


	// Fill face indices
	indices.reserve(3*mesh->mNumFaces);
	for (unsigned int i=0; i<mesh->mNumFaces; i++){
		// Assume the model has only triangles.
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	
	// The "scene" pointer will be deleted automatically by "importer"

}

#endif