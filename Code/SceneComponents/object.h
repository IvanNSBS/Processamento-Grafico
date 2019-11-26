#pragma once

#ifndef OBJECTH
#define OBJECTH

#include "material.h"
#include "hitrecord.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include "vec2.h"
#include "matrix44.h"

#define min_x 0
#define max_x 1
#define min_y 2
#define max_y 3
#define min_z 4
#define max_z 5
#define PI 3.141592653589793

std::default_random_engine generator; 
std::uniform_real_distribution<float> distributor(0, 1);
//Calcula uma direcao aleatoria dentro de uma esfera
vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = ( vec3( distributor(generator), distributor(generator), distributor(generator) ) * 2.0 )- vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}

// float drand48()
// {
//     return distributor(generator);
// }

//O objeto é uma esfera.

class Object;

class Object {
    public:
    Material* material;

    public:
    // Construtor
    Object() : material(nullptr){}
    Object(vec3 c, Material* mat): center(c), material(mat) {}
    vec3 center;

    /*
    Retorna true se r intersecta a cena e falso caso contrario
    Guarda possiveis informacoes sobre a interseccao em info, caso seja provido 
    */
    virtual bool intersect(const Ray& r, float &tmin, float &tmax, HitRecord& rec) const = 0;
};

class Sphere : public Object
{
    public:
    float radius;
    float radius2;

    Sphere() : Object(0, nullptr), radius(0), radius2(0){}
    Sphere(const vec3 &c, const float &rad, Material* mat): Object(c, mat), radius(rad), radius2(rad*rad) {}

    virtual bool intersect(const Ray& r, float &tmin, float &tmax, HitRecord& rec) const
    {
        vec3 nhit = this->center - r.getOrigin(); 
        float tca = dot(nhit, r.getDirection()); 
            if (tca < 0) 
                return false; 

        float d2 = dot(nhit, nhit) - tca * tca; 
            if (d2 > radius*radius) 
                return false; 
                    
        float thc = sqrt(radius*radius - d2); 

        float t0 = tca - thc; 
        float t1 = tca + thc;

        float tnear;
        if(t0 < 0)
            tnear = t1;
        else
            tnear = t0 < t1 ? t0 : t1;

        if(tnear < tmin || tnear > tmax)
            return false;
        
        rec.t = tnear;
        rec.phit = r.getOrigin() + r.getDirection() * tnear;
        rec.nhit = rec.phit - center;
        rec.nhit.make_unit_vector();
        rec.mat = this->material;
        rec.hitted = (Object*)this;
        return true; 
    }
};

struct Triangle {
public:
	vec3 vert[3];
	vec2 uv[3];
	vec3 normal[3];

	Triangle( const std::vector<vec3> &v, const std::vector<vec2> &t, const std::vector<vec3> &n)
	{
		vert[0] = v[0];
		vert[1] = v[1];
		vert[2] = v[2];
		uv[0] = t[0];
		uv[1] = t[1];
		uv[2] = t[2];
		normal[0] = n[0];
		normal[1] = n[1];
		normal[2] = n[2];
	}

	~Triangle(){}
};

class Mesh : public Object
{
public:
    std::vector<Triangle> tris;
	std::vector<Triangle> v_bbox;
    vec3 bbox_center;
    Mesh() : Object(0, nullptr){}
    Mesh(const vec3 &c, const vec3 &scale, const vec3& rot, Material* mat, std::string filepath): Object(c, mat){
        if(load_mesh_from_file( filepath.c_str() )){
            this->scale(scale);
            translate(c);
            rot_x(rot.x());
            rot_y(rot.y());
            rot_z(rot.z());
        }
    }

	void build_bbox(std::vector<vec3> verts)
	{
		int i = 0;
		for(auto v : verts){
			std::cout << "vert" << i << " = " << v << std::endl;
			i++;
		}
		vec3 v2v0 = unit_vector(verts[2] - verts[0]);
		vec3 v1v0 = unit_vector(verts[1] - verts[0]);
		vec3 n_front = -cross(v2v0, v1v0);
		vec3 v5v1 = unit_vector(verts[5] - verts[1]);
		vec3 v4v1 = unit_vector(verts[4] - verts[1]);
		vec3 n_right = cross(v5v1, v4v1);

		vec3 v4v0 = unit_vector(verts[4] - verts[0]);
		vec3 n_bottom = -cross(v4v0, v1v0);

		vec3 n_left = -n_right;
		vec3 n_top = -n_bottom;
		vec3 n_back = -n_front;

		vec3 n_v0 = (n_left + n_bottom + n_front)/3.0f;
		vec3 n_v1 = (n_right + n_bottom + n_front)/3.0f;

		vec3 n_v2 = (n_front + n_top + n_right)/3.0f;
		vec3 n_v3 = (n_front + n_top + n_left)/3.0f;

		vec3 n_v4 = (n_bottom + n_back + n_right)/3.0f;
		vec3 n_v7 = (n_bottom + n_back + n_left)/3.0f;

		vec3 n_v5 = (n_top + n_back + n_right)/3.0f;
		vec3 n_v6 = (n_top + n_back + n_left)/3.0f;

		std::vector<vec2> vts = { vec2(0,0), vec2(0,0) };

		std::vector<vec3> front1 = {verts[2], verts[1], verts[0] };
		std::vector<vec3> front2 = {verts[0], verts[3], verts[2] };
		std::vector<vec3> n_front1 = { n_v2, n_v1, n_v0 };
		std::vector<vec3> n_front2 = { n_v0, n_v3, n_v2 };
		v_bbox.emplace_back(front1, vts, n_front1);
		v_bbox.emplace_back(front2, vts, n_front2);


		std::vector<vec3> back1 = {verts[7], verts[6], verts[5] };
		std::vector<vec3> back2 = {verts[5], verts[4], verts[7] };
		std::vector<vec3> n_back1 = { n_v7, n_v6, n_v5 };
		std::vector<vec3> n_back2 = { n_v5, n_v4, n_v7 };
		v_bbox.emplace_back(back1, vts, n_back1);
		v_bbox.emplace_back(back2, vts, n_back2);


		std::vector<vec3> right1 = {verts[5], verts[4], verts[1] };
		std::vector<vec3> right2 = {verts[1], verts[2], verts[5] };
		std::vector<vec3> n_right1 = { n_v5, n_v4, n_v1 };
		std::vector<vec3> n_right2 = { n_v1, n_v2, n_v5 };
		v_bbox.emplace_back(right1, vts, n_right1);
		v_bbox.emplace_back(right2, vts, n_right2);


		std::vector<vec3> left1 = {verts[0], verts[3], verts[6] };
		std::vector<vec3> left2 = {verts[6], verts[7], verts[0] };
		std::vector<vec3> n_left1 = { n_v0, n_v3, n_v6 };
		std::vector<vec3> n_left2 = { n_v6, n_v7, n_v0 };
		v_bbox.emplace_back(left1, vts, n_left1);
		v_bbox.emplace_back(left2, vts, n_left2);


		std::vector<vec3> top1 = {verts[3], verts[6], verts[5] };
		std::vector<vec3> top2 = {verts[5], verts[2], verts[3] };
		std::vector<vec3> n_top1 = { n_v3, n_v6, n_v5 };
		std::vector<vec3> n_top2 = { n_v5, n_v2, n_v3 };
		v_bbox.emplace_back(top1, vts, n_top1);
		v_bbox.emplace_back(top2, vts, n_top2);


		std::vector<vec3> bottom1 = {verts[0], verts[7], verts[4] };
		std::vector<vec3> bottom2 = {verts[4], verts[1], verts[0] };
		std::vector<vec3> n_bottom1 = { n_v0, n_v7, n_v4 };
		std::vector<vec3> n_bottom2 = { n_v4, n_v1, n_v0 };
		v_bbox.emplace_back(bottom1, vts, n_bottom1);
		v_bbox.emplace_back(bottom2, vts, n_bottom2);
	}

    void translate(vec3 tl) {

		matrix44 tr(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					tl.x(), tl.y(), tl.z(), 1);
		for ( Triangle &tri : tris) {
			tr.mult_point_matrix(tri.vert[0], tri.vert[0]);
			tr.mult_point_matrix(tri.vert[1], tri.vert[1]);
			tr.mult_point_matrix(tri.vert[2], tri.vert[2]);
		}
		for ( Triangle &tri : v_bbox) {
			tr.mult_point_matrix(tri.vert[0], tri.vert[0]);
			tr.mult_point_matrix(tri.vert[1], tri.vert[1]);
			tr.mult_point_matrix(tri.vert[2], tri.vert[2]);
		}
		tr.mult_point_matrix(bbox_center, bbox_center);
	}

    void scale( const vec3 &t ) {
		matrix44 tr(t.x(), 0, 0, 0,
					0, t.y(), 0, 0,
					0, 0, t.z(), 0,
					0, 0, 0, 1);
		for ( Triangle &tri : tris) {
			tr.mult_point_matrix(tri.vert[0], tri.vert[0]);
			tr.mult_point_matrix(tri.vert[1], tri.vert[1]);
			tr.mult_point_matrix(tri.vert[2], tri.vert[2]);

			// tr.mult_point_matrix(tri.normal[0], tri.normal[0]);
			// tr.mult_point_matrix(tri.normal[1], tri.normal[1]);
			// tr.mult_point_matrix(tri.normal[2], tri.normal[2]);
		}
		for ( Triangle &tri : v_bbox) {
			tr.mult_point_matrix(tri.vert[0], tri.vert[0]);
			tr.mult_point_matrix(tri.vert[1], tri.vert[1]);
			tr.mult_point_matrix(tri.vert[2], tri.vert[2]);
		}
		tr.mult_point_matrix(bbox_center, bbox_center);
	}

    void rot_x(float deg) {

		matrix44 tr(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
			-bbox_center.x(), -bbox_center.y(), -bbox_center.z(), 1);
		matrix44 itr = tr.inverse();
		float sen = sin(deg*PI / 180.0);
		float co = cos(deg*PI / 180);
		matrix44 rot(1, 0, 0, 0,
					0, co, -sen, 0,
					0, sen, co, 0,
					0, 0, 0, 1);
		matrix44 result = (tr*rot)*itr;

		for ( Triangle &tri : tris) {
			result.mult_point_matrix(tri.vert[0], tri.vert[0]);
			result.mult_point_matrix(tri.vert[1], tri.vert[1]);
			result.mult_point_matrix(tri.vert[2], tri.vert[2]);

			result.mult_vec_matrix(tri.normal[0], tri.normal[0]);
			result.mult_vec_matrix(tri.normal[1], tri.normal[1]);
			result.mult_vec_matrix(tri.normal[2], tri.normal[2]);
		}

		for ( Triangle &tri : v_bbox) {
			result.mult_point_matrix(tri.vert[0], tri.vert[0]);
			result.mult_point_matrix(tri.vert[1], tri.vert[1]);
			result.mult_point_matrix(tri.vert[2], tri.vert[2]);

			result.mult_vec_matrix(tri.normal[0], tri.normal[0]);
			result.mult_vec_matrix(tri.normal[1], tri.normal[1]);
			result.mult_vec_matrix(tri.normal[2], tri.normal[2]);
		}
		result.mult_point_matrix(bbox_center, bbox_center);
	}

    void rot_y(float deg) {

		matrix44 tr(		1,			  0,			0,			0,
							0,			  1,			0,			0,
							0,			  0,			1,			0,
					-bbox_center.x(), -bbox_center.y(), -bbox_center.z(), 1);
		matrix44 itr = tr.inverse();
		float sen = sin(deg*PI / 180.0f);
		float co = cos(deg*PI / 180.0f);
		matrix44 rot(co, 0, sen, 0,
					0, 1, 0, 0,
					-sen, 0, co, 0,
					0, 0, 0, 1);
		matrix44 result = (tr*rot)*itr;

		for ( Triangle &tri : tris) {
			result.mult_point_matrix(tri.vert[0], tri.vert[0]);
			result.mult_point_matrix(tri.vert[1], tri.vert[1]);
			result.mult_point_matrix(tri.vert[2], tri.vert[2]);

			result.mult_vec_matrix(tri.normal[0], tri.normal[0]);
			result.mult_vec_matrix(tri.normal[1], tri.normal[1]);
			result.mult_vec_matrix(tri.normal[2], tri.normal[2]);
		}
		for ( Triangle &tri : v_bbox) {
			result.mult_point_matrix(tri.vert[0], tri.vert[0]);
			result.mult_point_matrix(tri.vert[1], tri.vert[1]);
			result.mult_point_matrix(tri.vert[2], tri.vert[2]);

			result.mult_vec_matrix(tri.normal[0], tri.normal[0]);
			result.mult_vec_matrix(tri.normal[1], tri.normal[1]);
			result.mult_vec_matrix(tri.normal[2], tri.normal[2]);
		}
		result.mult_point_matrix(bbox_center, bbox_center);

	}

    void rot_z(float deg) {

		matrix44 tr(		1,			  0,			0,			0,
							0,			  1,			0,			0,
							0,			  0,			1,			0,
					-bbox_center.x(), -bbox_center.y(), -bbox_center.z(), 1);
		matrix44 itr = tr.inverse();
		float sen = sin(deg*PI / 180.0f);
		float co = cos(deg*PI / 180.0f);
		matrix44 rot(co, -sen,  0, 0,
					 sen,  co,  0, 0,
                       0,   0,  1, 0,
                       0,   0,  0, 1);
		matrix44 result = (tr*rot)*itr;

		for ( Triangle &tri : tris) {
			result.mult_point_matrix(tri.vert[0], tri.vert[0]);
			result.mult_point_matrix(tri.vert[1], tri.vert[1]);
			result.mult_point_matrix(tri.vert[2], tri.vert[2]);
            
			result.mult_vec_matrix(tri.normal[0], tri.normal[0]);
			result.mult_vec_matrix(tri.normal[1], tri.normal[1]);
			result.mult_vec_matrix(tri.normal[2], tri.normal[2]);
		}
		for ( Triangle &tri : v_bbox) {
			result.mult_point_matrix(tri.vert[0], tri.vert[0]);
			result.mult_point_matrix(tri.vert[1], tri.vert[1]);
			result.mult_point_matrix(tri.vert[2], tri.vert[2]);
            
			result.mult_vec_matrix(tri.normal[0], tri.normal[0]);
			result.mult_vec_matrix(tri.normal[1], tri.normal[1]);
			result.mult_vec_matrix(tri.normal[2], tri.normal[2]);
		}
		result.mult_point_matrix(bbox_center, bbox_center);

	}

    bool intersect_triangle( Triangle tr, const Ray &r, HitRecord &rec ) const {
        const float EPSILON = 0.0000001;
        vec3 vertex0 = tr.vert[0];
        vec3 vertex1 = tr.vert[1];  
        vec3 vertex2 = tr.vert[2];
        vec3 edge1, edge2, h, s, q;
        float a,f,u,v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = cross(r.getDirection(), edge2);
        a = dot( edge1, h );
        if (a > -EPSILON && a < EPSILON){
            return false;    // This ray is parallel to this triangle.
        }
        f = 1.0/a;
        s = r.getOrigin() - vertex0;
        u = f *dot(s,h);
        if (u < 0.0 || u > 1.0)
            return false;
        q = cross(s, edge1);
        v = f * dot(r.getDirection(), q);
        if (v < 0.0 || u + v > 1.0)
            return false;
        // At this stage we can compute t to find out where the intersection point is on the line.
        float t = f * dot(edge2, q);
        if (t > EPSILON) // ray intersection
        {
            float w = 1.0f - u - v;

            rec.hitted = (Object*)this;
            rec.mat = this->material;
            rec.t = t;
			// w u v -> done (not good)
			// w v u -> next (seems good)
			// u w v -> done
			// u v w -> done
			// v u w -> done
			// v w u
            rec.nhit =  w*tr.normal[0] + u*tr.normal[1] + v*tr.normal[2];
            rec.phit = r.getOrigin() + (r.getDirection() * t);
            return true;
        }
        else {
            return false;
        }// This means that there is a line intersection but not a ray intersection.
    }

    virtual bool intersect(const Ray& r, float &tmin, float &tmax, HitRecord& rec) const {
        const float EPSILON = 0.0000001;
        rec.t = tmax;
        bool intersect = false;
		bool interesct_bbox = false;
		for ( Triangle tr : this->v_bbox){
            HitRecord ph;
            if(intersect_triangle( tr, r, ph )){
                interesct_bbox = true;
                if (rec.t > ph.t && ph.t > tmin && ph.t < tmax)
                    rec = ph;
            }
        }
		if(!interesct_bbox)
			return false;
			
        for ( Triangle tr : this->tris){
            HitRecord ph;
            if(intersect_triangle( tr, r, ph )){
                intersect = true;
                if (rec.t > ph.t && ph.t > tmin && ph.t < tmax)
                    rec = ph;
            }
        }
        return intersect;
    }

    bool load_mesh_from_file(const char* path) 
	{
		tris.clear();

		float bbox[6];
		bbox[min_x] = 999999.0f;
		bbox[max_x] = -999999.0f;
		bbox[min_y] = 999999.0f;
		bbox[max_y] = -999999.0f;
		bbox[min_z] = 999999.0f;
		bbox[max_z] = -999999.0f;

		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< vec3 > temp_vertices;
		std::vector< vec2 > temp_uvs;
		std::vector< vec3 > temp_normals;

		std::ifstream f(path);
		if (!f.is_open())
		{
			std::cout << "File cannot be oppened or does not exist\n";
			return false;
		}

		std::cout << "file was  oppened!\n";

		
		while (!f.eof())
		{
			char line[256];
			f.getline(line, 256);

			std::stringstream s;
			s << line;

			char junk;

			if ( line[0] == 'v')
			{
				if (line[1] == 't') 
				{
					vec2 uv;
					s >> junk >> junk >> uv[0] >> uv[1];
					temp_uvs.push_back(uv);
				}
				if (line[1] == 'n') 
				{
					vec3 normal;
					s >> junk >> junk >> normal[0] >> normal[1] >> normal[2];
					temp_normals.push_back(normal);
				}
				else {
					vec3 vertex;
					s >> junk >> vertex[0] >> vertex[1] >> vertex[2];
					temp_vertices.push_back(vertex);
				}
			}

			else if ( line[0] == 'f')
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				s >> junk >> vertex1 >> vertex2 >> vertex3;
				int fstslash = vertex1.find("/");
				int sndslash = vertex1.find("/", fstslash+1);
				int trdslash = vertex1.find("/", sndslash+1);
				std::string fst = vertex1.substr(0, fstslash);
				std::string snd = vertex1.substr(fstslash+1, sndslash - fstslash - 1);
				std::string trd = vertex1.substr(sndslash+1);
				vertexIndex[0] = atoi( fst.c_str() );
				uvIndex[0] = atoi(snd.c_str());
				normalIndex[0] = atoi( trd.c_str() );

				fstslash = vertex2.find("/");
				sndslash = vertex2.find("/", fstslash + 1);
				trdslash = vertex2.find("/", sndslash + 1);
				fst = vertex2.substr(0, fstslash);
				snd = vertex2.substr(fstslash + 1, sndslash - fstslash - 1);
				trd = vertex2.substr(sndslash + 1);
				vertexIndex[1] = atoi(fst.c_str());
				uvIndex[1] = atoi(snd.c_str());
				normalIndex[1] = atoi(trd.c_str());

				fstslash = vertex3.find("/");
				sndslash = vertex3.find("/", fstslash + 1);
				trdslash = vertex3.find("/", sndslash + 1);
				fst = vertex3.substr(0, fstslash);
				snd = vertex3.substr(fstslash + 1, sndslash - fstslash - 1);
				trd = vertex3.substr(sndslash + 1);
				vertexIndex[2] = atoi(fst.c_str());
				uvIndex[2] = atoi(snd.c_str());
				normalIndex[2] = atoi(trd.c_str());

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}
		
		auto update_bbox = [&](std::vector<vec3> verts){
			for(vec3 v : verts){
			if (v.x() < bbox[min_x])
				bbox[min_x] = v.x();
			if (v.x() > bbox[max_x])
				bbox[max_x] = v.x();

			if (v.y() < bbox[min_y])
				bbox[min_y] = v.y();
			if (v.y() > bbox[max_y])
				bbox[max_y] = v.y();

			if (v.z() < bbox[min_z])
				bbox[min_z] = v.z();
			if (v.z() > bbox[max_z])
				bbox[max_z] = v.z();
			}
		};

		for (unsigned int i = 0; i < vertexIndices.size(); i+=3)
		{
			unsigned int v1 = vertexIndices[i];
			unsigned int v2 = vertexIndices[i+1];
			unsigned int v3 = vertexIndices[i+2];

			unsigned int n1 = normalIndices[i];
			unsigned int n2 = normalIndices[i+1];
			unsigned int n3 = normalIndices[i+2];

			unsigned int uv1 = uvIndices[i];
			unsigned int uv2 = uvIndices[i+1];
			unsigned int uv3 = uvIndices[i+2];

			std::vector<vec3> vertices;
			vertices.push_back(temp_vertices[v1 - 1]);
			vertices.push_back(temp_vertices[v2 - 1]);
			vertices.push_back(temp_vertices[v3 - 1]);

			std::vector<vec2> uvs;
			if( uvIndices.size() > 0 ){
				uvs.push_back(temp_uvs[uv1 - 1]);
				uvs.push_back(temp_uvs[uv2 - 1]);
				uvs.push_back(temp_uvs[uv3 - 1]);
			}

			std::vector<vec3> normals;
			if( normalIndices.size() > 0){

				normals.push_back(temp_normals[n1 - 1]);
				normals.push_back(temp_normals[n2 - 1]);
				normals.push_back(temp_normals[n3 - 1]);
			}

			Triangle t(vertices, uvs, normals);
			tris.push_back(t);
			update_bbox(vertices);
		}

		bbox_center = vec3( 
			(bbox[max_x] + bbox[min_x]) / 2.0f, 
			(bbox[max_y] + bbox[min_y]) / 2.0f, 
			(bbox[max_z] + bbox[min_z]) / 2.0f);

	
		float mx = bbox[min_x];
		float Mx = bbox[max_x];

		float my = bbox[min_y];
		float My = bbox[max_y];

		float mz = bbox[min_z];
		float Mz = bbox[max_z];
		vec3 v0(mx, my, Mz);
		vec3 v1(Mx, my, Mz);
		vec3 v2(Mx, My, Mz);
		vec3 v3(mx, My, Mz);
		vec3 v4(Mx, my, mz);
		vec3 v5(Mx, My, mz);
		vec3 v6(mx, My, mz);
		vec3 v7(mx, my, mz);
		std::vector<vec3> verts = {v0, v1, v2, v3, v4, v5, v6, v7};
		build_bbox(verts);
		std::cout << "vertSize = " << vertexIndices.size() << "\n";
		std::cout << "normalSize = " << normalIndices.size() << "\n";
		std::cout << "uvSize = " << uvIndices.size() << "\n";

		return true;
	} 
        
};

#endif