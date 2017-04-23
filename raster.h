#pragma once
#include <vector>
#include <Eigen/Dense>

using namespace Eigen;
using std::vector;



struct Vertex
{
	Vertex& operator= (const Vertex &v)
	{
		for(int i=0; i<3; ++i) data[i]=v[i];
		return *this;
	}

	float& operator[] (const size_t i) { return data[i]; }
	const float& operator[] (const size_t i) const { return data[i]; }
private:
	float data[3];
};



struct Face
{
	int& operator[] (const size_t i) { return data[i]; }
	const int& operator[] (const size_t i) const { return data[i]; }
private:
	int data[3];
};



struct Mesh
{
	vector<Vertex> vertices;
	vector<Face> faces;
};



struct BoundingBox
{
	int xmin;
	int xmax;
	int ymin;
	int ymax;
};



struct DepthPix
{
	DepthPix(int x, int y, float d):x(x), y(y), depth(d) {}

	int x;
	int y;
	float depth;
};



bool compare(const DepthPix &pix1, const DepthPix &pix2)
{
	if(pix1.x<pix2.x) return true;
	else if(pix1.x>pix2.x) return false;
	else if(pix1.y<pix2.y) return true;
	else return false;
}


bool equal(DepthPix &pix1, DepthPix &pix2)
{
	if(pix1.x==pix2.x && pix1.y==pix2.y){
		if(pix1.depth>pix2.depth) pix1.depth = pix2.depth;
		else pix2.depth = pix1.depth;
		return true;
	}else return false;
}


class Raster
{
public:
	Raster(const Mesh &mesh)
	{
		for(size_t i=0; i<mesh.faces.size(); ++i)
		{
			Vertex tri[3];
			BoundingBox bbox;
			for(int j=0; j<3; ++j)
			{
				tri[j] = mesh.vertices[ mesh.faces[i][j] ];
			}
			boundingbox(tri, bbox);

			for(int x=bbox.xmin; x<=bbox.xmax; ++x)
				for(int y=bbox.ymin; y<=bbox.ymax; ++y)
				{
					float bc_coord[3];
					barycenticCoord(x, y, tri, bc_coord);
					if(bc_coord[0]>0&&bc_coord[1]>0&&bc_coord[2]>0){
						DepthPix dp(x, y, 0.0f);
						for(int j=0; j<3; ++j) dp.depth += bc_coord[j]*tri[j][2];
						buffer.push_back(dp);
					}
				}
		}

		sort(buffer.begin(), buffer.end(), compare);
		vector<DepthPix>::iterator it = unique(buffer.begin(), buffer.end(), equal);
		buffer.erase(it, buffer.end());
	}


	void boundingbox(const Vertex tri[3], BoundingBox &bbox)
	{
		int x=0, y=1;
		bbox.xmin = bbox.xmax = tri[0][x];
		bbox.ymin = bbox.ymax = tri[0][y];
		for(int i=1; i<3; ++i)
		{
			if(bbox.xmin>tri[i][x]) bbox.xmin=tri[i][x];
			if(bbox.xmax<tri[i][x]) bbox.xmax=tri[i][x];
			if(bbox.ymin>tri[i][y]) bbox.ymin=tri[i][y];
			if(bbox.ymax<tri[i][y]) bbox.ymax=tri[i][y];
		}
	}


	void barycenticCoord(int x, int y, const Vertex tri[3], float bc_coord[3])
	{
		Matrix3f A;
		Vector3f b;

		int xx=0, yy=1;

		A << tri[0][xx], tri[1][xx], tri[2][xx],
			 tri[0][yy], tri[1][yy], tri[2][yy],
			 1,          1,          1;
		b << x,          y,          1;
		
		Vector3f ans = A.colPivHouseholderQr().solve(b);
		
		for(int i=0; i<3; ++i) bc_coord[i] = ans[i];
	}



	const vector<DepthPix>& getBuffer() { return buffer; }



private:
	vector<DepthPix> buffer;
};