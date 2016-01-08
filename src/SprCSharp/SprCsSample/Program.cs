﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using SprCs;

namespace SprCsSample {
    class Program {
        static string def = "itvacfros";
        static string inc = "A";	// include: "A" for all
        static string exc = "f";	// exclude:

        static void Main(string[] args) {
            if (inc.Equals("A")) {
                inc = def;
            }
            if (check_test("i"))  test_intrinsic();
            if (check_test("t"))  test_tostring();
            if (check_test("v"))  test_vector();
            if (check_test("a"))  test_array();
            if (check_test("c"))  test_type_conv();
            if (check_test("f"))  test_func_args();
            if (check_test("r"))  test_func_return();
            if (check_test("o"))  test_operator();
            if (check_test("s"))  test_simulation();
        }

        static bool check_test(string chk) {
            return inc.Contains(chk) && !exc.Contains(chk);
        }

	static void title(string title) {
            System.Console.WriteLine("\n---[ " + title + " ]---");
	}

	static void put(string title, string expected, object obj) {
	    string line = title + ": expected: " + expected + ", result: " + obj;
	    System.Console.WriteLine(line);
	}

        static void test_intrinsic() {
            title("intrinsic");

            // intrinsic member
            PHSceneDesc descScene = new PHSceneDesc();
            PHRaycastHit raycastHit = new PHRaycastHit();
            GRVertexElement vertexelm = new GRVertexElement();
            // simple
            vertexelm.offset = 123;              put("short ", "123  ", vertexelm.offset);
            descScene.numIteration = 123;        put("int   ", "123  ", descScene.numIteration);
            descScene.bCCDEnabled = true;        put("bool  ", "True ", descScene.bCCDEnabled);
            raycastHit.distance = 0.123F;        put("float ", "0.123", raycastHit.distance);
            descScene.airResistanceRate = 0.123; put("double", "0.123", descScene.airResistanceRate);
            // nested
            descScene.gravity.x = 0;
            descScene.gravity.y = 0;
            descScene.gravity.z = -4.5;
/**/        put("set by elm", "(0.0, 0.0, -4.5)", descScene.gravity);
            descScene.gravity = new Vec3d(2.5, -5.2, 0.5);
/**/        put("set struct", "(2.5, -5.2, 0.5)", descScene.gravity);
        }

        static void test_tostring() {
            title("ToString");

            Vec3d v3d = new Vec3d(0.1, 0.2, 0.3);
            //string s = v3d.ToString();
/**/        put("ToString", "(0.1, 0.2, 0.3)", v3d.ToString());
/**/        System.Console.WriteLine("ToString: implicit ToString call           : " + v3d);

            PHSceneDesc descScene = new PHSceneDesc();
            PHSolidDesc descSolid = new PHSolidDesc();
            PHSdkIf phSdk = PHSdkIf.CreateSdk();
            PHSceneIf phScene = phSdk.CreateScene(descScene);
            PHSolidIf phSolid = phScene.CreateSolid(descSolid);
            phSolid.SetPose(new Posed(1, 0, 0, 0, 0, 2, 0));
            System.Console.WriteLine("ToString: phSolid:");
            System.Console.Write(phSolid.ToString());

            FWWinBaseDesc descWinBase = new FWWinBaseDesc();
            FWSdkIf fwSdk = FWSdkIf.CreateSdk();
            System.Console.WriteLine("ToString: fwSdk.ToString:");
            System.Console.Write(fwSdk.ToString());
        }

        static void test_vector() {
            title("vector");

            // vector member
            PHFemMeshNewDesc descFemMeshNew = new PHFemMeshNewDesc();
            vectorwrapper_int tets = descFemMeshNew.tets;
            // intrinsic element
            tets.push_back(101);
            tets.push_back(102);
            put("vec<int>", "2  ", tets.size());
            put("vec<int>", "101", tets[0]);
            put("vec<int>", "102", tets[1]);
            tets.clear();
            tets.push_back(101);
            tets.push_back(102);
            tets[0] = 201;
            tets[1] = 202;
            tets.push_back(203);
            put("vec<int>", "3  ", tets.size());
            put("vec<int>", "201", tets[0]);
            put("vec<int>", "202", tets[1]);
            put("vec<int>", "203", tets[2]);
            // structure element
            vectorwrapper_PHFemMeshNewDesc_Vec3d vertices = descFemMeshNew.vertices;
            vertices.push_back(new Vec3d(0.1, 0.2, 0.3));
            vertices.push_back(new Vec3d(0.4, 0.5, 0.6));
            put("vec<Vec3d>", "2  ", vertices.size());
            put("vec<Vec3d>", "0.1", vertices[0].x);
            put("vec<Vec3d>", "0.2", vertices[0].y);
            put("vec<Vec3d>", "0.3", vertices[0].z);
            put("vec<Vec3d>", "(0.4, 0.5, 0.6)", vertices[1].ToString());
        }

        static void test_array() {
            title("array");

            PHOpObjDesc descOpObj = new PHOpObjDesc();
            GRMeshFace meshFace = new GRMeshFace();
            for (int i = 0; i < 4; i++) {
                meshFace.indices[i] = 100 + i;
            }
            for (int i = 0; i < 4; i++) {
                System.Console.WriteLine("array<int>: expected: " + (100 + i) + " result: " + meshFace.indices[i]);
            }
            arraywrapper_int iarray = new arraywrapper_int(4);
            for (int i = 0; i < 4; i++)
            {
                meshFace.indices[i] = 200 + i;
            }
            meshFace.indices = iarray;
            for (int i = 0; i < 4; i++)
            {
                System.Console.WriteLine("array<int>: expected: " + (200 + i) + " result: " + meshFace.indices[i]);
            }
            //Vec3i v3i = new Vec3i(1, 2, 3);
            //System.Console.WriteLine("array<int> alloc(3): result:   " + v3i);
            //v3i.alloc(2);
            //for (int i = 0; i < 2; i++) {
            //    v3i[i].x = 10 * (i + 1);
            //    v3i[i].y = 10 * (i + 1);
            //    v3i[i].z = 10 * (i + 1);
            //}
            //System.Console.WriteLine("                     expected: " + "((10, 10, 10), (20, 20, 20))");
            //System.Console.WriteLine("                     result:   " + v3i);

        }

        static void test_type_conv() {
            title("type conversion");

            string msg_f3 = "f2d: (1, 2, 3)";
            string msg_d3 = "d2f: (4, 5, 6)";

            Vec3f f3 = new Vec3f(1, 2, 3);
            Vec3d d3 = new Vec3d(4, 5, 6);
            put("type_conv", msg_f3, f3);
//            print_vec3f(d3, msg_f3);    // This cause CS1502 and CS1503 compile error. <- OK
            put("type_conv", msg_d3, (Vec3f) d3);

            Vec3fStruct f3s = f3;
            Vec3dStruct d3s = d3;
            put("type_conv", msg_f3, "(" + f3s.x + ", " + f3s.y + ", " + f3s.z + ")");
//            print_vec3fs(d3s, msg_d3);  // This cause CS1502 and CS1503 compile error. <- OK
            put("type_conv", msg_d3,
                "(" + ((Vec3fStruct)d3s).x + ", " + ((Vec3fStruct)d3s).y + ", " + ((Vec3fStruct)d3s).z + ")");

            // -----
            // 戻り値の自動型変換のテスト
            PHSdkIf phSdk = PHSdkIf.CreateSdk();
            CDBoxDesc descBox = new CDBoxDesc();
            descBox.boxsize = new Vec3f(1,4,9);
            CDShapeIf shape = phSdk.CreateShape(CDBoxIf.GetIfInfoStatic(), descBox);
            // ↑ CreateShapeはCDBoxIfオブジェクトを返す。それをCDShapeIfで受ける。

            // CDShapeIf型の変数に格納されているが中身はCDBoxIfなので型変換可能。
            CDBoxIf box = shape as CDBoxIf;
            put("type_conv", "(1, 4, 9)", box.GetBoxSize().ToString());

            // CDBoxIf is not a CDSphereIf なので nullになることが期待される。
            CDSphereIf sphere = shape as CDSphereIf;
            if (sphere == null) {
                put("type_conv", "null", "null");
            } else {
                put("type_conv", "null", sphere.ToString());
            }
            
            // CDBoxIf is a CDConvexIf なのでnullにはならず型変換される。
            CDConvexIf convex = shape as CDConvexIf;
            if (convex == null) {
                put("type_conv", "not null", "null");
            } else {
                put("type_conv", "not null", convex.ToString());
            }

            // -----
            PHSceneDesc descScn = new PHSceneDesc();
            PHSceneIf scene = phSdk.CreateObject(PHSceneIf.GetIfInfoStatic(), descScn) as PHSceneIf;
            System.Console.WriteLine((scene == null) ? "null" : scene.ToString());
        }

        static void test_func_args() {
            title("function arguments");

            // ここでのコードは正常には動作はしない － 例外が起きて停止する。
            // デバッガで止めて値を確認すること。
            //
            // [int]
            CDShapePairIf shapePairIf = new CDShapePairIf();
            shapePairIf.GetShape(123);
        }

        static void test_func_return() {
            title("function return");
            int memoryLeakTest = 0;

            PHSceneDesc descScene = new PHSceneDesc();
            PHSolidDesc descSolid = new PHSolidDesc();
            if (memoryLeakTest == 1) return;

            PHSdkIf phSdk = PHSdkIf.CreateSdk();        // ここでメモリリークする
            if (memoryLeakTest == 2) return;
            PHSceneIf phScene = phSdk.CreateScene(descScene);

            descSolid.mass = 2.0;
            descSolid.inertia = new Matrix3d(2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0);
            PHSolidIf phSolid = phScene.CreateSolid(descSolid);
            descSolid.mass = 1e20f;

            descSolid.inertia = new Matrix3d(1e20f, 0.0, 0.0, 0.0, 1e20f, 0.0, 0.0, 0.0, 1e20f);
            PHSolidIf solid1 = phScene.CreateSolid(descSolid);

            PHHapticPointerDesc descHaptic = new PHHapticPointerDesc();
            PHHapticPointerIf phHaptic = phScene.CreateHapticPointer();

            //HISdkDesc descHi = new HISdkDesc();
            //HISdkIf hiSdk = HISdkIf.CreateSdk();

            put("ret int   ", "3    ", phScene.NSolids());
            for (int i = 0; i < 20; i++) {
                phScene.Step();
            }
            put("ret Uint  ", "20   ", phScene.GetCount());
            phHaptic.SetLocalRange(2.345f);
            put("ret float ", "2.345", phHaptic.GetLocalRange());
            put("ret double", "0.005", phScene.GetTimeStep());
            phScene.SetMaxVelocity(1.23);
            put("ret double", "0.123", phScene.GetMaxVelocity());
            phScene.EnableContactDetection(false);
            put("ret bool  ", "False", phScene.IsContactDetectionEnabled());
            phScene.EnableContactDetection(true);
            put("ret bool  ", "True ", phScene.IsContactDetectionEnabled());

            put("ret size_t", "152? ", phScene.GetDescSize());

            put("ret Vec3d ", "(0.0, -9.8, 0.0)", phScene.GetGravity());
            phScene.SetGravity(new Vec3d(0.1, -9.9, 0.2));
            put("ret Vec3d ", "(0.1, -9.9, 0.2)", phScene.GetGravity());
        }

        static void test_operator() {
            title("operator");

            // vector
            Vec3f v3a = new Vec3f(0.1f, 0.2f, 0.3f);
            Vec3f v3b = new Vec3f(0.4f, 0.5f, 0.6f);
            Vec3f v3c = v3a;
            Vec3f v3d;
	    put("vector unary  -", "(-0.1, -0.2, -0.3)", -v3a);
            put("vector binary +", "( 0.5,  0.7,  0.9)", v3a + v3b);
            put("vector binary -", "(-0.3, -0.3, -0.3)", v3a - v3b);
            put("vector binary *", "( 0.2,  0.4,  0.6)", v3a * 2);
            put("vector binary *", "( 0.8,  1.0,  1.2)", 2 * v3b);
            put("vector binary /", "( 0.05, 0.10,  0.15)", v3a / 2);
            put("vector binary *", "  0.32", v3a * v3b);
            put("vector binary %", "(-0.03, 0.06, -0.03)", v3a % v3b);
            put("vector binary ^", "(-0.03, 0.06, -0.03)", v3a ^ v3b);
            v3d = v3a; v3d += v3b;
            put("vector binary +=", "( 0.5,  0.7,  0.9)", v3d);
            v3d = v3a; v3d -= v3b;
            put("vector binary -=", "(-0.3, -0.3, -0.3)", v3d);
            v3d = v3a; v3d *= 2;
            put("vector binary *=", "( 0.2,  0.4,  0.6)", v3d);
            v3d = v3a; v3d /= 2;
            put("vector binary /=", "(0.05, 0.10, 0.15)", v3d);
            put("vector binary ==", "True ", v3a == v3c);
            put("vector binary ==", "False", v3a == v3b);
            put("vector binary !=", "True ", v3a != v3b);
            put("vector binary !=", "False", v3a != v3c);
	}

        static void test_simulation() {
            title("physical simulation");

            PHSceneDesc descScene = new PHSceneDesc();
            PHSolidDesc descSolid = new PHSolidDesc();
            CDBoxDesc descBox = new CDBoxDesc();
            PHSdkIf phSdk = PHSdkIf.CreateSdk();
            PHSceneIf phScene = phSdk.CreateScene(descScene);
            PHSolidIf phSolid = phScene.CreateSolid(descSolid);
            phSolid.AddShape(phSdk.CreateShape(CDBoxIf.GetIfInfoStatic(), descBox));
            phSolid.SetPose(new Posed(1, 0, 0, 0, 0, 2, 0));

            PHSolidIf phFloor = phScene.CreateSolid(descSolid);
            phFloor.SetDynamical(false);
            descBox.boxsize = new Vec3f(10, 10, 10);
            phFloor.AddShape(phSdk.CreateShape(CDBoxIf.GetIfInfoStatic(), descBox));
            phFloor.SetPose(new Posed(1, 0, 0, 0, 0, -5, 0));

            PHBallJointDesc descJoint = new PHBallJointDesc();
            PHBallJointIf j = phScene.CreateJoint(phFloor, phSolid, PHBallJointIf.GetIfInfoStatic(), descJoint).Cast();
            System.Console.WriteLine(j.GetName());

            /*
            for (int i = 0; i < 200; i++) {
                phScene.Step();
                //System.Console.WriteLine(i.ToString() + " : " + phSolid.GetPose());
                System.Console.WriteLine(String.Format("{0, 3}", i) + " : " + phSolid.GetPose());
            }
            */
        }

    }
}
