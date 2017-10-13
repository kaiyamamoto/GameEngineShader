//--------------------------------------------------------------------------------------
// ファイル名: Collision.cpp
// 作成者:
// 作成日:
// 説明:当たり判定ライブラリ
//--------------------------------------------------------------------------------------
 
//----------------------------------------------
//	include
//----------------------------------------------
#include	"Collision.h"

// ネームスペースの省略指定
using namespace DirectX::SimpleMath;
using namespace MyLibrary;

//typedef DirectX::SimpleMath::Matrix Matrix;

//----------------------------------------------
//	using
//----------------------------------------------

//NS_COLLISION_BEGIN

//----------------------------------------------
//	define
//----------------------------------------------

//----------------------------------------------
//	static work
//----------------------------------------------

//----------------------------------------------
//	static functions prototype
//----------------------------------------------
 
//----------------------------------------------
//	static functions definition
//----------------------------------------------

//----------------------------------------------
//	global functions definition
//----------------------------------------------
////--------------------------------------------------------------------------------------------
//// 説　明 : ベクトルのスカラー三重積を計算
//// 引　数 : _v0		ベクトル0
////			_v1		ベクトル1
////			_v2		ベクトル2
//// 戻り値 : スカラー三重積
//// メ　モ : 
////--------------------------------------------------------------------------------------------
//inline float VectorTriple(const Vector3&_v0, const Vector3&_v1, const Vector3&_v2)
//{
//	Vector3 m;
//	
//	m = _v0.Cross(_v1);
//	
//	return m.Dot(_v2);
//}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 点と線分の距離の二乗を取得
// 引　数 : _point	点
//			_segment	線分
// 戻り値 : 点と線分の距離の二乗
// メ　モ : 平方根を算出しない版
//	線分の端点をA,B	点をCとして、
//	①ABベクトルとACベクトルの内積が負の時、点Aが点Cの最近傍である
//	②BAベクトルとBCベクトルの内積が負の時、点Bが点Cの最近傍である
//	③　①、②に該当しない場合、点Cの射影がACの内側に存在するため、その点が最近傍である
//--------------------------------------------------------------------------------------------
float GetSqDistancePoint2Segment(const Vector3& _point, const Segment& _segment)
{
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
	Vector3 SegmentSub;
	Vector3 SegmentPoint;
	Vector3 CP;
	
	// 線分の始点から終点へのベクトル
	SegmentSub = _segment.end - _segment.start;
	
	// 線分の始点から点へのベクトル
	SegmentPoint = _point - _segment.start;
	if ( SegmentSub.Dot(SegmentPoint) < epsilon)
	{// ２ベクトルの内積が負なら、線分の始点が最近傍
		return SegmentPoint.Dot(SegmentPoint);
	}

	// 点から線分の終点へのベクトル
	SegmentPoint = _segment.end - _point;
	if (SegmentSub.Dot(SegmentPoint) < epsilon)
	{// ２ベクトルの内積が負なら、線分の終点が最近傍
		return SegmentPoint.Dot(SegmentPoint);
	}
	
	// 上記のどちらにも該当しない場合、線分上に落とした射影が最近傍
	// (本来ならサインで求めるが、外積の大きさ/線分のベクトルの大きさで求まる)
	CP = SegmentSub.Cross(SegmentPoint);

	return CP.Dot(CP) / SegmentSub.Dot(SegmentSub);
}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 線分と線分の距離の二乗を取得
// 引　数 : _segment0	線分0
//			_segment1	線分1
// 戻り値 : 線分と線分の距離の二乗
// メ　モ : 平方根を算出しない版
//	①直線上の最接近点が両方の線分の内側に存在する時
//	②直線上の最接近点が一方のみの線分の内側に存在する時
//	③直線上の最接近点が両方の線分の外側に存在する時
//	それぞれのケースで二つの線分上の最接近点を求め、その距離を算出する
//--------------------------------------------------------------------------------------------
float GetSqDistanceSegment2Segment(const Segment& _segment0, const Segment& _segment1)
{
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
	Vector3 d0,d1,r;
	Vector3 c0,c1;	// 二つの線分上の最接近点
	Vector3 v;		// c1→c0ベクトル
	float a,b,c,e,f;
	float s,t;
	float denom;
	float tnom;
	
	d0 = _segment0.end - _segment0.start;	// 線分0の方向ベクトル
	d1 = _segment1.end - _segment1.start; // 線分1の方向ベクトル
	r = _segment0.start - _segment1.start; // 線分1の始点から線分0の始点へのベクトル
	a = d0.Dot(d0);		// 線分0の距離の二乗
	e = d1.Dot(d1);		// 線分1の距離の二乗
//	b = d0.Dot(d1);		// 最適化の為後方に移動した
//	c = d0.Dot(r);		// 最適化の為後方に移動した
//	f = d1.Dot(r);		// 最適化の為後方に移動した
	
	// いづれかの線分の長さが0かどうかチェック
	if ( a <= epsilon && e <= epsilon )
	{// 両方長さ0
		v = _segment0.start -_segment1.start;
	
		return v.Dot(v);
	}
	
	if ( a <= epsilon )
	{// 線分0が長さ0
		return GetSqDistancePoint2Segment( _segment0.start, _segment1 );
	}
	
	if ( e <= epsilon )
	{// 線分1が長さ0
		return GetSqDistancePoint2Segment( _segment1.start, _segment0 );
	}
	
	b = d0.Dot(d1);
	f = d1.Dot(r);
	c = d0.Dot(r);
	
	denom = a * e - b * b;	// 常に非負
	// 線分が平行でない場合、直線0上の直線1に対する最近接点を計算、そして
	// 線分0上にクランプ。そうでない場合は任意のsを選択
	if ( denom != 0 )
	{
		s = Clamp( (b * f - c * e) /denom, 0, 1 );
	}
	else
	{
		s = 0;
	}
	
	// 直線1上の最接近点を計算
	tnom = b * s + f;
	
	if ( tnom < 0 )
	{
		t = 0;
		s = Clamp( -c / a , 0, 1 );
	}
	else if ( tnom > e )
	{
		t = 1;
		s = Clamp( (b-c)/a, 0, 1 );
	}
	else
	{
		t = tnom/e;
	}
	
	c0 = s * d0 + _segment0.start;
	c1 = t * d1 + _segment1.start;
	v = c0 - c1;
	
	return v.Dot(v);
}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 点と線分の最近接点を計算
// 引　数 : _point	点
//			_segment	線分
//			_closest	最近接点(結果出力用）
// 戻り値 : なし
// メ　モ : 
//--------------------------------------------------------------------------------------------
void ClosestPtPoint2Segment(const Vector3& _point, const Segment& _segment, Vector3* _closest)
{
	Vector3 segv;
	Vector3 s2p;
	float t;
	
	segv = _segment.end - _segment.start;
	s2p = _point - _segment.start;
	t = segv.Dot(s2p) / segv.Dot(segv);

	// 線分の外側にある場合、tを最接近点までクランプ
	t = Clamp( t, 0, 1 );

	// クランプされているtからの射影されている位置を計算
	*_closest = t * segv + _segment.start;
}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 点と三角形の最近接点を計算
// 引　数 : _point	点
//			_segment	線分
//			_closest	最近接点(結果出力用）
// 戻り値 : なし
// メ　モ : 
//--------------------------------------------------------------------------------------------
void ClosestPtPoint2Triangle(const Vector3& _point, const Triangle& _triangle, Vector3* _closest)
{
	// _pointがP0の外側の頂点領域の中にあるかどうかチェック
	Vector3 P0_P1 = _triangle.P1 - _triangle.P0;
	Vector3 P0_P2 = _triangle.P2 - _triangle.P0;
	Vector3 P0_PT = _point - _triangle.P0;

	float d1 = P0_P1.Dot(P0_PT);
	float d2 = P0_P2.Dot(P0_PT);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// P0が最近傍
		*_closest = _triangle.P0;
		return;
	}

	// _pointがP1の外側の頂点領域の中にあるかどうかチェック
	Vector3 P1_PT = _point - _triangle.P1;

	float d3 = P0_P1.Dot(P1_PT);
	float d4 = P0_P2.Dot(P1_PT);

	if (d3 >= 0.0f && d4 <= d3)
	{
		// P1が最近傍
		*_closest = _triangle.P1;
		return;
	}

	// _pointがP0_P1の辺領域の中にあるかどうかチェックし、あれば_pointのP0_P1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*_closest = _triangle.P0 + v * P0_P1;
		return;
	}

	// _pointがP2の外側の頂点領域の中にあるかどうかチェック
	Vector3 P2_PT = _point - _triangle.P2;

	float d5 = P0_P1.Dot(P2_PT);
	float d6 = P0_P2.Dot(P2_PT);
	if (d6 >= 0.0f && d5 <= d6)
	{
		*_closest = _triangle.P2;
		return;
	}

	// _pointがP0_P2の辺領域の中にあるかどうかチェックし、あれば_pointのP0_P2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*_closest = _triangle.P0 + w * P0_P2;
		return;
	}

	// _pointがP1_P2の辺領域の中にあるかどうかチェックし、あれば_pointのP1_P2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*_closest = _triangle.P1 + w * (_triangle.P2 - _triangle.P1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*_closest = _triangle.P0 + P0_P1 * v + P0_P2 * w;
}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 線分と線分の最近接点を計算
// 引　数 : _segment0	線分0
//			_segment1	線分1
//			_closest0	線分0上の最近接点(結果出力用）
//			_closest1	線分1上の最近接点(結果出力用）
// 戻り値 : なし
// メ　モ : 
//--------------------------------------------------------------------------------------------
void ClosestPtSegment2Segment(const Segment& _segment0, const Segment& _segment1, Vector3* _closest0, Vector3* _closest1)
{
	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
	Vector3 d0,d1,r;
	float a,b,c,e,f;
	float s,t;
	float denom;
	float tnom;
	
	d0 = _segment0.end - _segment0.start;		// 線分0の方向ベクトル
	d1 = _segment1.end - _segment1.start;		// 線分1の方向ベクトル
	r = _segment0.start - _segment1.start;	// 線分1の始点から線分0の始点へのベクトル
	a = d0.Dot(d0);	// 線分0の距離の二乗
	e = d1.Dot(d1);	// 線分1の距離の二乗
//	b = d0.Dot(d1);	// 最適化の為後方に移動した
//	c = d0.Dot(r);	// 最適化の為後方に移動した
//	f = d1.Dot(r);	// 最適化の為後方に移動した
	
	// いづれかの線分の長さが0かどうかチェック
	if ( a <= epsilon && e <= epsilon )
	{// 両方長さ0
		*_closest0 = _segment0.start;
		*_closest1 = _segment1.start;
		return;
	}
	
	if ( a <= epsilon )
	{// 線分0が長さ0
		*_closest0 = _segment0.start;
		ClosestPtPoint2Segment( _segment0.start, _segment1, _closest1 );
	}
	
	if ( e <= epsilon )
	{// 線分1が長さ0
		*_closest1 = _segment1.start;
		ClosestPtPoint2Segment( _segment1.start, _segment0, _closest0 );
	}
	
	b = d0.Dot(d1);
	f = d1.Dot(r);
	c = d0.Dot(r);
	
	denom = a * e - b * b;	// 常に非負
	// 線分が平行でない場合、直線0上の直線1に対する最近接点を計算、そして
	// 線分0上にクランプ。そうでない場合は任意のsを選択
	if ( denom != 0 )
	{
		s = Clamp( (b * f - c * e)/denom, 0, 1 );
	}
	else
	{
		s = 0;
	}
	
	// 直線1上の最接近点を計算
	tnom = b * s + f;
	
	if ( tnom < 0 )
	{
		t = 0;
		s = Clamp( -c / a, 0, 1 );
	}
	else if ( tnom > e )
	{
		t = 1;
		s = Clamp( (b-c) / a, 0, 1 );
	}
	else
	{
		t = tnom / e;
	}
	
	*_closest0 = s * d0 + _segment0.start;
	*_closest1 = t * d1 + _segment1.start;
}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : 点とAABBの最近接点を計算
//// 引　数 : _point	点
////			_aabb	AABB
////			_closest	最近接点(結果出力用）
//// 戻り値 : なし
//// メ　モ : 
////--------------------------------------------------------------------------------------------
//void ClosestPtPoint2AABB(const Vector3& _point, const AABB& _aabb, Vector3 *_closest)
//{
//	// 各座標軸に対して、点の座標値がボックスの外側にある場合に
//	// ボックスまで点をクランプ
//	_closest->x = Clamp( _point.x, _aabb.Min.x, _aabb.Max.x );
//	_closest->y = Clamp( _point.y, _aabb.Min.y, _aabb.Max.y );
//	_closest->z = Clamp( _point.z, _aabb.Min.z, _aabb.Max.z );
//}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : 点とOBBの最近接点を計算
//// 引　数 : _point	点
////			_obb	OBB
////			_closest	最近接点(結果出力用）
//// 戻り値 : なし
//// メ　モ : 
////--------------------------------------------------------------------------------------------
//void ClosestPtPoint2OBB(const Vector3& _point, const OBB& _obb, Vector3 *_closest)
//{
//	Vector3 d;
//	float dist;
//	float *size = (float*)&_obb.Size;
//	
//	d = _point - _obb.center;	// ボックス中心から点へのベクトル
//	
//	// ボックスの中心から開始して、各軸にそって進めていく
//	*_closest = _obb.center;
//	
//	for ( int i = 0; i < 3; i++ )
//	{
//		// dを軸に射影して
//		// ボックスの中心からdの軸に沿った距離を得る
//		dist = d.Dot(_obb.Axis[i]);
//		
//		// ボックスの範囲より距離が大きい場合、ボックスまでクランプ
//		dist = Clamp( dist, -size[i], -size[i] );
//		
//		*_closest = dist * _obb.Axis[i] + *_closest;
//	}
//}
//
//--------------------------------------------------------------------------------------------
// 説　明 : ３点から三角形を計算（反時計回りに指定）
// 引　数 : _p0	点０
//			_p1	点１
//			_p2 点２
//			_triangle 三角形(結果出力用）
// 戻り値 : なし
// メ　モ : 反時計回り順に３点指定する
//--------------------------------------------------------------------------------------------
void ComputeTriangle(const Vector3& _p0, const Vector3& _p1, const Vector3& _p2, Triangle* _triangle)
{
	_triangle->P0 = _p0;
	_triangle->P1 = _p1;
	_triangle->P2 = _p2;

	Vector3 P0_P1 = _triangle->P1 - _triangle->P0;
	Vector3 P1_P2 = _triangle->P2 - _triangle->P1;

	_triangle->Normal = P0_P1.Cross(P1_P2);
	_triangle->Normal.Normalize();
}

////--------------------------------------------------------------------------------------------
//// 説　明 : AABBとAABBの当たりチェック
//// 引　数 : _box0	ボックス0
////			_box1	ボックス1
////			_inter	擬似交点（最近接点同士の中点）
//// 戻り値 : 交差しているか否か
//// メ　モ : 
////--------------------------------------------------------------------------------------------
//bool CheckAABB2AABB(const AABB& _box0, const AABB& _box1, Vector3 *_inter)
//{
//	// ある軸に沿って分離している場合は交差がないので終了
//	if ( _box0.Max.x < _box1.Min.x || _box0.Min.x > _box1.Max.x )	return false;
//	if ( _box0.Max.y < _box1.Min.y || _box0.Min.y > _box1.Max.y )	return false;
//	if ( _box0.Max.z < _box1.Min.z || _box0.Min.z > _box1.Max.z )	return false;
//	
//	if ( _inter )
//	{// 擬似交点を計算
//		Vector3 center0,center1;
//		Vector3 cpoint0, cpoint1;
//		
//		center0 = (_box0.Min + _box0.Max) / 2.0f;
//		
//		center1 = (_box1.Min + _box1.Max) / 2.0f;
//
//		// 互いの中心点との最接近点を求める
//		ClosestPtPoint2AABB( center1, _box0, &cpoint0 );
//		ClosestPtPoint2AABB( center0, _box1, &cpoint1 );
//		
//		*_inter = (cpoint0 + cpoint1) / 2.0f;
//	}
//	// 全ての軸に沿って重なっているので、当たっている
//	return true;
//}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : OBBとOBBの当たりチェック
//// 引　数 : _box0	ボックス0
////			_box1	ボックス1
////			_inter	擬似交点（最近接点同士の中点）
//// 戻り値 : 交差しているか否か
//// メ　モ : 計１５の分離軸上にボックスの射影を落とし、
////			そのうち一つでも重なっていないものがあればOBBは交差していない
////--------------------------------------------------------------------------------------------
//bool CheckOBB2OBB(const OBB&_box0, const OBB&_box1, Vector3 *_inter)
//{
//	const float epsilon = 1.0e-5f;	// 誤差吸収用の微小な値
//	float ra, rb;
//	Matrix R, AbsR;
//	Vector3 t;
//	float *size0 = (float*)&_box0.Size;	// ベクトルの要素に添え字でアクセスする為にポインタをキャストする
//	float *size1 = (float*)&_box1.Size;
//	float *tf = (float*)&t;
//	float d;
//	
//	// 各軸の内積を計算
//	for (int i = 0; i < 3; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			R.m[i][j] = _box0.Axis[i].Dot(_box1.Axis[j]);
//		}
//	}
//	
//			
//	// 平行移動ベクトルtを計算（ボックス中心から中心へのベクトル）	
//	t = _box1.center - _box0.center;
//	
//	// 平行移動をボックス0の座標フレームに変換
//	t = Vector3(t.Dot(_box0.Axis[0]), t.Dot(_box0.Axis[1]), t.Dot(_box0.Axis[2]));
//	
//	// 共通の部分式を計算
//	// ２つの辺が平行でそれらの外積がゼロベクトルになる時に
//	// 演算エラーが起きないようにイプシロンの項を追加
//	for (int i = 0; i < 3; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			AbsR.m[i][j] = fabsf(R.m[i][j]) + epsilon;
//		}
//	}
//			
//	//////////////////////////////////////////////////////////////
//			
//	// 分離軸候補をボックス0の各軸として判定
//	for (int i = 0; i < 3; i++)
//	{
//		ra = size0[i];
//		rb = size1[0] * AbsR.m[i][0] + size1[1] * AbsR.m[i][1] + size1[2] * AbsR.m[i][2];
//		if (fabsf(tf[i]) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス1の各軸として判定
//	for (int i = 0; i < 3; i++)
//	{		
//		ra = size0[0] * AbsR.m[0][i] + size0[1] * AbsR.m[1][i] + size0[2] * AbsR.m[2][i];
//		rb = size1[i];
//		d = tf[0] * R.m[0][i] + tf[1] * R.m[1][i] + tf[2] * R.m[2][i];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	//////////////////////////////////////////////////////////////
//	
//	// 分離軸候補をボックス0のX軸×ボックス1のX軸として判定
//	{
//		ra = size0[1] * AbsR.m[2][0] + size0[2] * AbsR.m[1][0];
//		rb = size1[1] * AbsR.m[0][2] + size1[2] * AbsR.m[0][1];
//		d = tf[2] * R.m[1][0] - tf[1] * R.m[2][0];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス0のX軸×ボックス1のY軸として判定
//	{
//		ra = size0[1] * AbsR.m[2][1] + size0[2] * AbsR.m[1][1];
//		rb = size1[0] * AbsR.m[0][2] + size1[2] * AbsR.m[0][0];
//		d = tf[2] * R.m[1][1] - tf[1] * R.m[2][1];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス0のX軸×ボックス1のZ軸として判定
//	{
//		ra = size0[1] * AbsR.m[2][2] + size0[2] * AbsR.m[1][2];
//		rb = size1[0] * AbsR.m[0][1] + size1[1] * AbsR.m[0][0];
//		d = tf[2] * R.m[1][2] - tf[1] * R.m[2][2];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	//////////////////////////////////////////////////////////////
//	
//	// 分離軸候補をボックス0のY軸×ボックス1のX軸として判定
//	{
//		ra = size0[0] * AbsR.m[2][0] + size0[2] * AbsR.m[0][0];
//		rb = size1[1] * AbsR.m[1][2] + size1[2] * AbsR.m[1][1];
//		d = tf[0] * R.m[2][0] - tf[2] * R.m[0][0];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス0のY軸×ボックス1のY軸として判定
//	{
//		ra = size0[0] * AbsR.m[2][1] + size0[2] * AbsR.m[0][1];
//		rb = size1[0] * AbsR.m[1][2] + size1[2] * AbsR.m[1][0];
//		d = tf[0] * R.m[2][1] - tf[2] * R.m[0][1];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス0のY軸×ボックス1のZ軸として判定
//	{
//		ra = size0[0] * AbsR.m[2][2] + size0[2] * AbsR.m[0][2];
//		rb = size1[0] * AbsR.m[1][1] + size1[1] * AbsR.m[1][0];
//		d = tf[0] * R.m[2][2] - tf[2] * R.m[0][2];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	//////////////////////////////////////////////////////////////
//	
//	// 分離軸候補をボックス0のZ軸×ボックス1のX軸として判定
//	{
//		ra = size0[0] * AbsR.m[1][0] + size0[1] * AbsR.m[0][0];
//		rb = size1[1] * AbsR.m[2][2] + size1[2] * AbsR.m[2][1];
//		d = tf[1] * R.m[0][0] - tf[0] * R.m[1][0];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス0のZ軸×ボックス1のY軸として判定
//	{
//		ra = size0[0] * AbsR.m[1][1] + size0[1] * AbsR.m[0][1];
//		rb = size1[0] * AbsR.m[2][2] + size1[2] * AbsR.m[2][0];
//		d = tf[1] * R.m[0][1] - tf[0] * R.m[1][1];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 分離軸候補をボックス0のZ軸×ボックス1のZ軸として判定
//	{
//		ra = size0[0] * AbsR.m[1][2] + size0[1] * AbsR.m[0][2];
//		rb = size1[0] * AbsR.m[2][1] + size1[1] * AbsR.m[2][0];
//		d = tf[1] * R.m[0][2] - tf[0] * R.m[1][2];
//		if (fabsf(d) > ra + rb )	return false;
//	}
//	
//	// 全ての軸に沿って重なっているので、当たっている
//	
//	if ( _inter )
//	{// 擬似交点を計算
//		Vector3 cpoint0,cpoint1;
//		
//		// 互いの中心点との最接近点を求める
//		ClosestPtPoint2OBB( _box1.center, _box0, &cpoint0 );
//		ClosestPtPoint2OBB( _box0.center, _box1, &cpoint1 );
//
//		*_inter = (cpoint0 + cpoint1) / 2.0f;
//	}
//	
//	return true;
//}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : 球と球の当たりチェック
//// 引　数 : _sphere0	球1
////			_sphere1	球2
////			_inter		擬似交点(中心間の中点）
//// 戻り値 : 交差しているか否か
//// メ　モ : 
////--------------------------------------------------------------------------------------------

// ベクトルの長さを計算
float VectorLength(const Vector3& v)
{
	float length;

	// 三平方の定理
	length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	return length;
}

// ベクトルの長さの二乗を計算
float VectorLengthSQ(const Vector3& v)
{
	float lengthsq;

	// 三平方の定理
	lengthsq = v.x * v.x + v.y * v.y + v.z * v.z;

	return lengthsq;
}

// ２点間の距離を計算
float Distance3D(const Vector3& p1, const Vector3& p2)
{
	Vector3 sub;

	sub = p1 - p2;

	// ベクトルの長さを計算する
	float distance = VectorLength(sub);
	
	return distance;
}

// ２点間の距離の二乗を計算
float DistanceSQ3D(const Vector3& p1, const Vector3& p2)
{
	Vector3 sub;

	sub = p1 - p2;

	// ベクトルの長さを計算する
	float distanceSQ = VectorLengthSQ(sub);

	return distanceSQ;
}

// 球と球の当たり判定
bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter)
{
	// 中心間の距離を計算する
	//float d = Vector3::Distance(_sphere0.center, _sphere1.center);
	//float d = Distance3D(sphere0.center, sphere1.center);
	//
	//// 距離が半径の和より大きければ当たっていない
	//if ( d > sphere0.radius + sphere1.radius ) return false;

	// 球AとBの中心座標の差を計算する
	Vector3 sub = sphereA.center - sphereB.center;

	//// 三平方の定理で、ベクトルの長さを計算する
	//float distance = sqrtf(sub.x * sub.x + sub.y * sub.y + sub.z * sub.z);

	//// 距離が半径の和より大きければ当たっていない
	//if (distance > sphereA.radius + sphereB.radius)
	//{
	//	return false;
	//}

	// 三平方の定理で、ベクトルの長さの二乗を計算する
	float distanceSQ = DistanceSQ3D(sphereA.center, sphereB.center);

	// 半径の和
	float radius_sum = sphereA.radius + sphereB.radius;
	float radius_sumSQ = radius_sum * radius_sum;

	// 距離の二乗が半径の和の二乗より大きければ当たっていない
	if (distanceSQ > radius_sumSQ)
	{
		return false;
	}
	
	// 変数アドレスを指定していた場合のみ擬似交点を計算する
	if ( inter )
	{
		// 球A、Bの中心座標を、半径の比率で内分した点を、疑似交点とする
		// Bから疑似交点へのベクトル
		Vector3 BtoInter = sub * sphereB.radius / (sphereA.radius + sphereB.radius);
		*inter = sphereB.center + BtoInter;
	}
	
	return true;
}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : 球と平面の当たりチェック
//// 引　数 : _sphere		球
////			_plane		平面
////			_inter		擬似交点（平面上の最接近点）
//// 戻り値 : 交差しているか否か
//// メ　モ : 
////--------------------------------------------------------------------------------------------
//bool CheckSphere2Plane(const Sphere& _sphere, const Plane& _plane, Vector3 *_inter)
//{
//	float dist;
//	
//	dist = _sphere.center.Dot(_plane.Normal); // 座標系の原点から球の中心座標への距離
//	dist -= _plane.Distance;	// 平面の原点距離を減算することで、平面と球の中心との距離が出る
//	// 距離の絶対値が半径より大きければ当たっていない
//	if ( fabsf( dist ) > _sphere.radius )	return false;
//	
//	if ( _inter )
//	{// 擬似交点を計算
//		*_inter = -dist * _plane.Normal + _sphere.center;
//	}
//	
//	return true;
//}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 球とカプセルの当たりチェック
// 引　数 : _sphere		球
//			_capsule	カプセル
//			_inter		擬似交点（球の中心とカプセルの芯線上の最接近点の中点）
// 戻り値 : 交差しているか否か
// メ　モ : 
//--------------------------------------------------------------------------------------------
bool CheckSphere2Capsule(const Sphere& _sphere, const Capsule& _capsule, Vector3* inter)
{
	// 球の中心とカプセルの線分の距離（の二乗）を計算
	float distance2 = GetSqDistancePoint2Segment( _sphere.center, _capsule.segment );
	
	// 距離（の二乗）が半径の和（の二乗）より小さければ当たっている
	float radius = _sphere.radius + _capsule.radius;
		
	if ( distance2 > radius*radius )	return false;
	
	if (inter)
	{// 擬似交点を計算
		Vector3 cpoint;
		
		ClosestPtPoint2Segment( _sphere.center, _capsule.segment, &cpoint );
		
		*inter = (_sphere.center + cpoint) / 2.0f;
	}

	return true;
}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : カプセルとカプセルの当たりチェック
//// 引　数 : _capsule0	カプセル0
////			_capsule1	カプセル1
////			_inter		擬似交点（カプセルの芯線上の最接近点同士の中点）
//// 戻り値 : 交差しているか否か
//// メ　モ : 
////--------------------------------------------------------------------------------------------
bool CheckCapsule2Capsule(const Capsule& _capsule0, const Capsule& _capsule1, Vector3* inter)
{
	// 球の中心とカプセルの線分の距離（の二乗）を計算
	float distanceSQ = GetSqDistanceSegment2Segment( _capsule0.segment, _capsule1.segment );
	
	// 半径の和（の二乗）を計算
	float radiusSum = _capsule0.radius + _capsule1.radius;
	float radiusSumSQ = radiusSum * radiusSum;
	
	// 距離（の二乗）が半径の和（の二乗）より大きければ当たっていない
	if (distanceSQ > radiusSumSQ)	return false;
	
	if (inter)
	{// 擬似交点を計算
		Vector3 cpoint0,cpoint1;
		
		ClosestPtSegment2Segment( _capsule0.segment, _capsule1.segment, &cpoint0, &cpoint1 );
		*inter = (cpoint0 + cpoint1) / 2.0f;
	}
	
	return true;
}
//
////--------------------------------------------------------------------------------------------
//// 説　明 : 直線と法線付き三角形の当たりチェック
//// 引　数 : _line		直線（start→endが正方向）
////			_tri		法線付き三角形
////			_inter		交点（省略可）
//// 戻り値 : 交差しているか否か
//// メ　モ : 直線に方向の区別はない
////--------------------------------------------------------------------------------------------
//bool CheckLine2Triangle(const Line& _line, const Triangle& _triangle, Vector3 *_inter)
//{
//	Vector3 LayV;	// 直線の点0→終点
//	Vector3 lt0;		// 直線の点0→三角形の頂点0
//	Vector3 lt1;		// 直線の点0→三角形の頂点1
//	Vector3 lt2;		// 直線の点0→三角形の頂点2
//	float u,v,w;
//
//	LayV = _line.point1 - _line.point0;	// 直線の方向ベクトルを取得
//	lt0 = _triangle.P0 - _line.point0;
//	lt1 = _triangle.P1 - _line.point0;
//	lt2 = _triangle.P2 - _line.point0;
//	
//	// 直線が３角形の三辺の内側を通るどうかを判定。
//	// 判定はスカラー三重積を用いて計算する符号付の六面体の体積が
//	// すべて正であるかどうかの判定による
//	u = VectorTriple( LayV, lt2, lt1 );
//	if( u < 0 )	return false;
//	
//	v = VectorTriple( LayV, lt0, lt2 );
//	if( v < 0 )	return false;
//	
//	w = VectorTriple( LayV, lt1, lt0 );
//	if( w < 0 )	return false;
//	
//	if ( _inter )
//	{// 交点を計算
//		if ( u == 0 && v == 0 && w == 0 )
//		{// ゼロ除算の為、別扱い
//			// 何もしない
//		}
//		else
//		{
//			float denom = 1.0f / ( u + v + w );
//			u = u * denom;
//			v = v * denom;
//			w = w * denom;
//		}
//		*_inter = u * _triangle.P0 + v * _triangle.P1 + w * _triangle.P2;
//	}
//	
//	return true;	
//}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 線分（有向）と法線付き三角形の当たりチェック
// 引　数 : _segment		光線の線分（start→endが正方向）
//			_tri		法線付き三角形
//			_inter		交点（省略可）
// 戻り値 : 交差しているか否か
// メ　モ : 裏面の当たりはとらない
//--------------------------------------------------------------------------------------------
bool CheckSegment2Triangle(const Segment& _segment, const Triangle& _triangle, Vector3 *_inter)
{
	const float epsilon = -1.0e-5f;	// 誤差吸収用の微小な値
	Vector3 	LayV;		// 線分の終点→始点
	Vector3 	tls;		// 三角形の頂点0→線分の始点
	Vector3 	tle;		// 三角形の頂点0→線分の終点
	float 	distl0;
	float 	distl1;
	float 	dp;
	float 	denom;
	float 	t;
	Vector3	s;			// 直線と平面との交点
	Vector3 	st0;		// 交点→三角形の頂点0
	Vector3 	st1;		// 交点→三角形の頂点1
	Vector3 	st2;		// 交点→三角形の頂点2
	Vector3 	t01;		// 三角形の頂点0→頂点1
	Vector3 	t12;		// 三角形の頂点1→頂点2
	Vector3 	t20;		// 三角形の頂点2→頂点0
	Vector3	m;

	// 線分の始点が三角系の裏側にあれば、当たらない
	tls = _segment.start - _triangle.P0;
	distl0 = tls.Dot(_triangle.Normal);	// 線分の始点と平面の距離
	if ( distl0 <= epsilon ) return false;
	
	// 線分の終点が三角系の表側にあれば、当たらない
	tle = _segment.end - _triangle.P0;
	distl1 = tle.Dot(_triangle.Normal);	// 線分の終点と平面の距離
	if ( distl1 >= -epsilon ) return false;
	
	// 直線と平面との交点sを取る
	denom = distl0 - distl1;
	t = distl0 / denom;
	LayV = _segment.end - _segment.start;	// 線分の方向ベクトルを取得
	s = t * LayV + _segment.start;
	
	// 交点が三角形の内側にあるかどうかを調べる。
	// 三角形の内側にある場合、交点から各頂点へのベクトルと各辺ベクトルの外積（三組）が、全て法線と同じ方向を向く
	// 一つでも方向が一致しなければ、当たらない。
	st0 = _triangle.P0 - s;
	t01 = _triangle.P1 - _triangle.P0;
	m = st0.Cross(t01);
	dp = m.Dot(_triangle.Normal);
	if ( dp <= epsilon ) return false;
	
	st1 = _triangle.P1 - s;
	t12 = _triangle.P2 - _triangle.P1;
	m = st1.Cross(t12);
	dp = m.Dot(_triangle.Normal);
	if ( dp <= epsilon ) return false;
	
	st2 = _triangle.P2 - s;
	t20 = _triangle.P0 - _triangle.P2;
	m = st2.Cross(t20);
	dp = m.Dot(_triangle.Normal);
	if ( dp <= epsilon ) return false;
	
	if ( _inter )
	{
		*_inter = s;	// 交点をコピー
	}
	
	return true;	
}

// 三角形と点の当たり判定
bool CheckPoint2Triangle(const Vector3& _point, const Triangle& _triangle)
{
	//点と三角形は同一平面上にあるものとしています。同一平面上に無い場合は正しい結果になりません
	//線上は外とみなします。
	//ABCが三角形かどうかのチェックは省略...

	// 点0→1、 1→2、 2→0 のベクトルをそれぞれ計算
	Vector3 v01 = _triangle.P1 - _triangle.P0;
	Vector3 v12 = _triangle.P2 - _triangle.P1;
	Vector3 v20 = _triangle.P0 - _triangle.P2;
	// 三角形の各頂点から点へのベクトルをそれぞれ計算
	Vector3 v0p = _point - _triangle.P0;
	Vector3 v1p = _point - _triangle.P1;
	Vector3 v2p = _point - _triangle.P2;
	// 各辺ベクトルと、点へのベクトルの外積を計算
	Vector3 c0 = v01.Cross(v0p);
	Vector3 c1 = v12.Cross(v1p);
	Vector3 c2 = v20.Cross(v2p);
	// 内積で同じ方向かどうか調べる
	float dot01 = c0.Dot(c1);
	float dot02 = c0.Dot(c2);
	// 外積ベクトルが全て同じ方向なら、三角形の内側に点がある
	if (dot01 > 0 && dot02 > 0)
	{
		return true;
	}

	//三角形の外側に点がある
	return false;

}
//
//--------------------------------------------------------------------------------------------
// 説　明 : 球と法線付き三角形の当たりチェック
// 引　数 : _sphere		球
//			_tri		法線付き三角形
//			_inter		交点（省略可）
// 戻り値 : 交差しているか否か
// メ　モ : 裏面の当たりはとらない
//--------------------------------------------------------------------------------------------
bool CheckSphere2Triangle(const Sphere& _sphere, const Triangle& _triangle, Vector3 *_inter)
{
	Vector3 p;

	// 球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(_sphere.center, _triangle, &p);

	Vector3 v = p - _sphere.center;

	// 球と三角形が交差するのは、球の中心から点pまでの距離が球の半径よりも小さい場合
	if (v.Dot(v) <= _sphere.radius * _sphere.radius)
	{
		if (_inter)
		{
			*_inter = p;
		}
		
		return true;
	}

	return false;

	// 球と平面（三角形が乗っている平面）の当たり判定
	// 球と平面の距離を計算
	float ds = _sphere.center.Dot(_triangle.Normal);
	float dt = _triangle.P0.Dot(_triangle.Normal);
	float dist = ds - dt;
	// 距離が半径以上なら、当たらない
	if (fabsf(dist) > _sphere.radius)	return false;
	// 中心点を平面に射影したとき、三角形の内側にあれば、当たっている
	// 射影した座標
	Vector3 center = -dist * _triangle.Normal + _sphere.center;

	// 三角形の外側になければ、当たっていない
	if (!CheckPoint2Triangle(center, _triangle))	return false;

	if (_inter)
	{
		*_inter = center;	// 交点をコピー
	}

	return true;
}
//
//NS_COLLISION_END