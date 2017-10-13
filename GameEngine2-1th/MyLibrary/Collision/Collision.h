//--------------------------------------------------------------------------------------
// ファイル名: Collision.h
// 作成者: 藤澤　伸治
// 作成日: 2016/06/07
// 説明:当たり判定ライブラリ
//      殆どのアルゴリズムは
//		「ゲームプログラミングのためのリアルタイム衝突判定」http://www.borndigital.co.jp/book/164.html　から引用
//--------------------------------------------------------------------------------------
#pragma once
#include <d3d11_1.h>
#include <SimpleMath.h>

namespace MyLibrary
{
	//----------------------------------------------
	//	define
	//----------------------------------------------


	//----------------------------------------------
	//	class
	//----------------------------------------------
	//// 円
	//class Circle
	//{
	//public:
	//	DirectX::SimpleMath::Vector3	center;	// 中心座標
	//	float	radius;	// 半径
	//	// コンストラクタ
	//	Circle()
	//	{
	//		radius = 1.0f;
	//	}
	//};

	//// 直線
	//class Line
	//{
	//public:
	//	DirectX::SimpleMath::Vector3	point0;	// 点0
	//	DirectX::SimpleMath::Vector3	point1;	// 点1
	//};

	// 線分
	class Segment
	{
	public:
		DirectX::SimpleMath::Vector3	start;	// 始点座標
		DirectX::SimpleMath::Vector3	end;	// 終点座標
	};

	//// 軸平行境界ボックス(AABB)
	//class AABB
	//{
	//public:
	//	Point	Max;	// X,Y,Z座標それぞれの最小値
	//	Point	Min;	// X,Y,Z座標それぞれの最大値
	//};

	//// 有向境界ボックス(OBB)
	//class OBB
	//{
	//public:
	//	Point	center;		// 中心座標
	//	Vector	Axis[3];	// ローカルX,Y,Z軸
	//	Vector	Size;		// 大きさ（X,Y,Zそれぞれ1辺の長さの半分）
	//};

	// 球
	class Sphere
	{
	public:
		DirectX::SimpleMath::Vector3	center;	// 中心座標
		float	radius;	// 半径
		// コンストラクタ
		Sphere()
		{
			radius = 1.0f;
		}
	};

	// カプセル(球をスウィープした形状）
	class Capsule
	{
	public:
		Segment	segment;	// 芯線
		float	radius;	// 半径
		// コンストラクタ
		Capsule()
		{
			segment.start = DirectX::SimpleMath::Vector3(0, 0, 0);
			segment.end = DirectX::SimpleMath::Vector3(0, 1, 0);
			radius = 1.0f;
		}
	};

	//// 平面
	//class Plane
	//{
	//public:
	//	Vector	Normal;		// 法線ベクトル
	//	float	Distance;	// 原点からの距離（平面上のある与えられた点pに対してd = Dot(n,p)が成立）
	//	// コンストラクタ
	//	Plane()
	//	{
	//		Distance = 0.0f;
	//	}
	//};

	// 法線付き三角形（反時計回りが表面）
	class Triangle
	{
	public:
		DirectX::SimpleMath::Vector3	P0;
		DirectX::SimpleMath::Vector3	P1;
		DirectX::SimpleMath::Vector3	P2;
		DirectX::SimpleMath::Vector3	Normal;	// 法線ベクトル
	};

	//----------------------------------------------
	//	global functions prototype
	//----------------------------------------------

	// 最小値と最大値の間にクランプする
	inline float Clamp(float _x, float _min, float _max)
	{
		return min(max(_x, _min), _max);
	}
	// ベクトルのスカラー三重積を計算
	//float VectorTriple(const Vector &_v0, const Vector &_v1, const Vector &_v2);

	////-- 距離
	//// 点と線分の距離の二乗を取得
	//float GetSqDistancePoint2Segment(const Point& _point, const Segment& _segment);
	//// 線分と線分の距離の二乗を取得
	//float GetSqDistanceSegment2Segment(const Segment& _segment0, const Segment& _segment1);

	//-- 最接近点の取得
	// 点と線分
	//void ClosestPtPoint2Segment(const Point& _point, const Segment& _segment, Point* _closest);
	//// 点と三角形
	//void ClosestPtPoint2Triangle(const Point& _point, const Triangle& _triangle, Point* _closest);
	//// 線分と線分の最近接点を計算
	//void ClosestPtSegment2Segment(const Segment& _segment0, const Segment& _segment1, Point* _closest0, Point* _closest1);
	//// 点とAABB
	//void ClosestPtPoint2AABB(const Point& _point, const AABB& _aabb, Point *_closest);
	//// 点とOBB
	//void ClosestPtPoint2OBB(const Point& _point, const OBB& _obb, Point *_closest);
	//
	//// ３点から三角形を計算（反時計回りに指定）
	void ComputeTriangle(const DirectX::SimpleMath::Vector3& _p0, const DirectX::SimpleMath::Vector3& _p1, const DirectX::SimpleMath::Vector3& _p2, Triangle* _triangle);
	//
	//// 当たり判定
	//bool CheckAABB2AABB(const AABB& _box0, const AABB& _box1, Point *_inter = nullptr);					// AABBとAABB
	//bool CheckOBB2OBB(const OBB& a, const OBB& b, Point *_inter = nullptr);								// OBBとOBB
	//bool CheckSphere2Sphere(const Sphere& _sphere0, const Sphere& _sphere1, Point *_inter = nullptr);	// 球と球
	//bool CheckSphere2Plane(const Sphere& _sphere, const Plane& _plane, Point *_inter = nullptr);		// 球と平面
	//bool CheckSphere2Capsule(const Sphere& _sphere, const Capsule& _capsule, Point *_inter = nullptr);	// 球とカプセル
	//bool CheckCapsule2Capsule(const Capsule& _capsule0, const Capsule& _capsule1, Point *_inter = nullptr);	// カプセルとカプセル
	//bool CheckLine2Triangle(const Line& _lay, const Triangle& _triangle, Point *_inter = nullptr);		// 直線と法線付き三角形// 三角形と点の当たり判定
	//bool CheckPoint2Triangle(const Point& _point, const Triangle& _triangle);	// 点が三角形の中にあるか判定
	bool CheckSegment2Triangle(const Segment& _segment, const Triangle& _triangle, DirectX::SimpleMath::Vector3 *_inter = nullptr);	// 線分と法線付き三角形
	bool CheckSphere2Triangle(const Sphere& _sphere, const Triangle& _triangle, DirectX::SimpleMath::Vector3 *_inter = nullptr); //球と法線付き三角形の当たりチェック
	bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::SimpleMath::Vector3* _inter = nullptr);
	bool CheckCapsule2Capsule(const Capsule& _capsule0, const Capsule& _capsule1, DirectX::SimpleMath::Vector3* _inter = nullptr);
	bool CheckSphere2Capsule(const Sphere& _sphere, const Capsule& _capsule, DirectX::SimpleMath::Vector3* _inter = nullptr);
}