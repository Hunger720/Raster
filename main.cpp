#include <iostream>
#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "raster.h"

using namespace std;

typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;



int main()
{
	Mesh mesh;
	string path;


	MyMesh rMesh;
	cout<<"please input the obj file path: ";
	getline(cin, path);
	try{
		if (!OpenMesh::IO::read_mesh(rMesh, path)){
			std::cerr<<"Cannot read mesh from "<<path<<'\n';
		}else{
			std::cout<<"Open "<<path<<" succeeded.\n";
		}
	}
	catch (std::exception& x){
		std::cerr << x.what() << std::endl;
	}
	
	for(MyMesh::VertexIter v_it=rMesh.vertices_begin(); v_it!=rMesh.vertices_end(); v_it++)
	{
		Vertex v;
		for(int i=0; i<3; ++i) v[i] = rMesh.point(*v_it)[i];
		mesh.vertices.push_back(v);
	}
		
	for(MyMesh::FaceIter f_it=rMesh.faces_begin(); f_it!=rMesh.faces_end(); f_it++)
	{
		int i = 0;
		Face f;
		for(MyMesh::FaceVertexIter fv_it=rMesh.fv_iter(*f_it); fv_it.is_valid(); fv_it++)
		{
			f[i++] = fv_it->idx();
		}
		mesh.faces.push_back(f);	
	}



	Raster raster(mesh);
	const vector<DepthPix> buffer = raster.getBuffer();



	MyMesh wMesh;
	MyMesh::VertexHandle *vhandle;
	vhandle = new MyMesh::VertexHandle[buffer.size()];
	for(unsigned int i=0; i<buffer.size(); ++i)
		vhandle[i] = wMesh.add_vertex(MyMesh::Point(buffer[i].x, buffer[i].y, buffer[i].depth));
	cout<<"please input the obj file path: ";
	getline(cin, path);
	try{
		if (!OpenMesh::IO::write_mesh(wMesh, path)){
			std::cerr << "Cannot write mesh to file "<<path<< std::endl;
		}else{
			std::cout<<"Save "<<path<<" succeeded.\n";
		}
	}
	catch (std::exception& x){
		std::cerr << x.what() << std::endl;
	}
}