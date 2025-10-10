#include "Include/WinHeaders.hpp"
#include "../SDK/SDK.h"
#include <shared_mutex>
#include <unordered_set>


namespace IronMan::Core
{
	using namespace physx;
	using namespace IronMan::Core::SDK;

	typedef enum RigidFreshType : int
	{
		RNone = 0,
		RPlayer,
		RAlways,
		RSlow,
		RStatic,
		RNotAdd
	};



#pragma pack(push, 1)
	struct GuTriangleMeshDataStruct
	{
		int mNbVertices;
		int triangleCount;
		ptr_t mVertices;
		ptr_t mPolygons;
		BYTE mTriangleMeshFlags;
		inline GuTriangleMeshDataStruct() :mNbVertices(0), mVertices(0), triangleCount(0), mPolygons(0), mTriangleMeshFlags(0) {}
		inline GuTriangleMeshDataStruct(int a, ptr_t b, int c, ptr_t d, BYTE e) :
			mNbVertices(a), mVertices(b), triangleCount(c), mPolygons(d), mTriangleMeshFlags(e) {}
	};

	struct GuHeightFieldDataStruct
	{
		int NumRows;
		int NumCols;
		float rowLimit;
		float colLimit;
		float nbColumns;
		int UnknownData;
		ptr_t Samples;
		inline GuHeightFieldDataStruct() :NumRows(0), NumCols(0), rowLimit(0.f), colLimit(0.f), nbColumns(0.f), Samples(0), UnknownData(0) {}
	};

	struct RigidUserData
	{
		RigidFreshType DataType;					//0x00
		__int64 RigidPoint;							//0x04
		__int64 TransformPtr;						//0x0C
		__int64 DynamicBufferedBody2WorldPtr;		//0x14
		PxTransform CalculateTransform;				//0x1C
		PxTransform DynamicBufferedBody2World;		//0x38
		int ReadCount = 0;							//0x54
		int ForceUpdate;
	};

	struct ShapeUserData
	{
		ptr_t ShapePtr;								//0x00
		int NbTriangles;							//0x08
		int NbVertices;								//0x0C
		ptr_t ShapeTransformPtr;					//0x10
		ptr_t SimulationFilterDataPtr;				//0x18
		ptr_t BoxVecPtr;
		ptr_t ScalePtr;
		PxTransform ShapeTransform;					//0x20
		PxFilterData SimulationFilterData;			//0x3C
		TArray<ptr_t> LogTArray;					//0x4C
		TArray<ptr_t> WriteTArray;					//0x5C
		PxVec3 BoxVec;
		PxMeshScale MeshScale;
		int ReadCount = 0;							//0x6C
		int ShapeType = 0;
		ptr_t RigidPoint;							//0x70
	};

	struct ShapeUserDataHeightField
	{
		ptr_t ShapePtr;								//0x00
		ptr_t RigidPoint;							//0x08
		TArray<ptr_t> LogTArray;					//0x10
		TArray<ptr_t> WriteTArray;					//0x20
		int NumRows;								//0x30
		int NumCols;								//0x34
		float rowLimit;								//0x38
		float colLimit;								//0x3C
		float nbColumns;							//0x40
		float RowScale;								//0x44
		float ColScale;								//0x48
		float HeightScale;							//0x4C
		ptr_t HeightField;							//0x50
		ptr_t Samples;								//0x58
		int ReadCount;								//0x5C
	};
#pragma pack(pop)
	struct DynamicInfo
	{
		ptr_t RigidBody;
		ptr_t UserData;
		inline DynamicInfo(ptr_t a, ptr_t b) :RigidBody(a), UserData(b) {}
	};

	struct GuTriangleMesh {
		std::vector<PxVec3> Vertices;      // 顶点数据
		std::vector<PxU16> Indices16;        // 索引数据
		std::vector<PxU32> Indices32;        // 索引数据
		int mNbVertices;
		int triangleCount;
		ptr_t mVertices;
		ptr_t mPolygons;
		ptr_t NbVerticesPtr;
		ptr_t NbtrianglePtr;
		ptr_t TriangleMeshPtr;
		BYTE mTriangleMeshFlags;
		int FirstV;
		int FirstV2;
		int FirstV3;
		float FirstF;
		float FirstF2;
		float FirstF3;
		inline GuTriangleMesh()
		{
			Vertices.resize(0);
			Indices16.resize(0);
			Indices32.resize(0);
			mNbVertices = 0;
			triangleCount = 0;
			mVertices = 0;
			mPolygons = 0;
			NbVerticesPtr = 0;
			NbtrianglePtr = 0;
			TriangleMeshPtr = 0;
			mTriangleMeshFlags = 0;
			FirstV = 0;
			FirstV2 = 0;
			FirstV3 = 0;
			FirstF = 0.f;
			FirstF2 = 0.f;
			FirstF3 = 0.f;
		}
	};

	struct GuHeightField {
		std::vector<PxHeightFieldSample> HeightData;
		int NumRows;
		int NumCols;
		float rowLimit;
		float colLimit;
		float nbColumns;
		int unknown;
		ptr_t Samples;
		ptr_t HolderPtr;
		ptr_t HeightField;
		float HeightScale;
		float RowScale;
		float ColScale;
		PxMeshGeometryFlags Flags;
		std::string HeightFieldName;
		inline GuHeightField() : NumRows(0), NumCols(0), RowScale(1.0f), ColScale(1.0f), HeightScale(1.0f), unknown(0), HolderPtr(0), HeightField(),
			rowLimit(0), colLimit(0), nbColumns(0), Samples(0), HeightFieldName("") {}

	};

	struct ShapeDataStruct {
		ptr_t Shape;
		ptr_t ShapeTransformPtr;
		PxTransform ShapeTransform;
		ptr_t ShapeGeometryPtr;
		PxGeometryHolder ShapeGeometry;
		PxShapeFlags ShapeFlags;
		ptr_t SimulationFilterDataPtr;
		PxFilterData SimulationFilterData;
		ptr_t QueryFilterDataPtr;
		PxFilterData QueryFilterData;
		GuTriangleMesh TriangleMesh;
		GuHeightField HeightField;
		bool FirstInitPtr;
		bool FirstInitGeo;
		int GeoType;
		int CmpGeoType;
		ShapeDataStruct() : Shape(0), ShapeTransformPtr(0), ShapeTransform(PxTransform()), ShapeGeometry(PxGeometryHolder()), ShapeFlags(0), SimulationFilterDataPtr(0), SimulationFilterData(PxFilterData()), TriangleMesh(GuTriangleMesh()),
			ShapeGeometryPtr(0), QueryFilterData(), QueryFilterDataPtr(0), GeoType(0), CmpGeoType(0), FirstInitGeo(false), FirstInitPtr(false) {}
		ShapeDataStruct(ptr_t a, ptr_t b, PxTransform c, PxGeometryHolder d, PxShapeFlags e, ptr_t f, PxFilterData g, GuTriangleMesh h)
			: Shape(a), ShapeTransformPtr(b), ShapeTransform(c), ShapeGeometry(d), ShapeFlags(e), SimulationFilterDataPtr(f), SimulationFilterData(g), TriangleMesh(h),
			ShapeGeometryPtr(0), QueryFilterData(), QueryFilterDataPtr(0), GeoType(0), CmpGeoType(0), FirstInitGeo(false), FirstInitPtr(false) {}
	};

	struct RigiBodyStruct {
		int Type;
		int MeshType;
		ptr_t Point;
		ptr_t TransformPtr;
		PxTransform Transform;
		ptr_t DynamicBufferedBody2WorldPtr;
		PxTransform DynamicBufferedBody2World;
		std::vector<ShapeDataStruct> Shapes;
		PxTransform CaculdateTransform;
		TArray<ptr_t>ShapeTArray;
		TArray<ptr_t>CmpShapeTArray;
		int ReadTimeCount;
		int CurrentReadTime;
		int LowSpeedReadTime;
		RigiBodyStruct() :Type(0), MeshType(0), Point(0), TransformPtr(0), DynamicBufferedBody2WorldPtr(0), DynamicBufferedBody2World(PxTransform()), Shapes(std::vector<ShapeDataStruct>()), ShapeTArray(TArray<ptr_t>()), ReadTimeCount(0), CurrentReadTime(0), LowSpeedReadTime(0) {}

		RigiBodyStruct(int a, ptr_t b, ptr_t c, PxTransform d, ptr_t e, PxTransform f, TArray<ptr_t>g, int h, int i)
			: Type(a), Point(b), TransformPtr(c), Transform(d), DynamicBufferedBody2WorldPtr(e), DynamicBufferedBody2World(f), ShapeTArray(g), MeshType(h), ReadTimeCount(0), CurrentReadTime(i), LowSpeedReadTime(0) {}
	};
}