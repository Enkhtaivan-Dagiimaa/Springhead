﻿using UnityEngine;
using System.Collections;
using System.Linq;
using SprCs;

public class CDRoundConeBehavior : SpringheadBehaviour {
    public CDRoundConeDescStruct roundConeDescripter = null;
    public GameObject child = null;

    void Reset() {
        SetDLLPath();
        roundConeDescripter = new CDRoundConeDesc();
    }

    Posed TransformToPose(Transform t) {
        Vector3 p = t.position;
        Quaternion q = t.rotation;
        return new Posed(q.w, q.x, q.y, q.z, p.x, p.y, p.z);
    }

    void Awake() {
        PHSceneIf phScene = gameObject.GetComponentInParent<PHSceneBehaviour>().GetPHScene();
        PHSdkIf phSdk = phScene.GetSdk();

        MeshRoundCone mrc = gameObject.GetComponent<MeshRoundCone>();
        if (mrc != null) {
            // RoundConeプリミティブを使う場合
            PHSolidBehaviour solidBehaviour = gameObject.GetComponentInParent<PHSolidBehaviour>();
            GameObject solidObject = solidBehaviour.gameObject;
            CDRoundConeDesc descRoundCone = new CDRoundConeDesc();
            descRoundCone.radius = new Vec2f(mrc.r1, mrc.r2);
            descRoundCone.length = mrc.length;
            solidBehaviour.phSolid.AddShape(phSdk.CreateShape(CDRoundConeIf.GetIfInfoStatic(), descRoundCone));

            // 剛体オブジェクトからの相対変換
            Posed relposeShapeSolid = TransformToPose(solidObject.transform).Inv() * TransformToPose(gameObject.transform);

            // SpringheadとUnityでカプセルの向きが違うことに対する補正
            Vec3f p = new Vec3f();
            if (mrc.pivot == MeshRoundCone.Pivot.R1) {
                p = new Vec3f(+0.5f * mrc.length, 0, 0);
            } else if (mrc.pivot == MeshRoundCone.Pivot.R2) {
                p = new Vec3f(-0.5f * mrc.length, 0, 0);
            }
            Quaterniond q = Quaterniond.Rot(90.0f * Mathf.Deg2Rad, new Vec3d(0, 1, 0));
            Posed corrCapsuleDir = new Posed(q.w, q.x, q.y, q.z, p.x, p.y, p.z);

            solidBehaviour.phSolid.SetShapePose(solidBehaviour.phSolid.NShape() - 1, relposeShapeSolid * corrCapsuleDir);

        } else {
            // Sphereプリミティブ２つを使う場合
            if (child == null) {
                foreach (Transform x in gameObject.transform) {
                    if (x.gameObject.GetComponentInChildren<SphereCollider>() != null) {
                        child = x.gameObject;
                        break;
                    }
                }
            }

            if (child == null) {
                Debug.LogError("child is null");
            } else {
                SphereCollider sc1 = gameObject.GetComponent<SphereCollider>();
                SphereCollider sc2 = child.GetComponent<SphereCollider>();
                Transform tr1 = gameObject.GetComponent<Transform>();
                Transform tr2 = child.GetComponent<Transform>();
                PHSolidBehaviour solidBehaviour = gameObject.GetComponentInParent<PHSolidBehaviour>();
                if (sc1 != null && sc2 != null && tr1 != null && tr2 != null && solidBehaviour != null && solidBehaviour.phSolid != null) {
                    CDRoundConeDesc descRoundCone = new CDRoundConeDesc();
                    Vector3 scale1 = gameObject.transform.lossyScale;
                    Vector3 scale2 = child.transform.lossyScale;

                    descRoundCone.radius = new Vec2f(sc2.radius * (Mathf.Max(Mathf.Max(scale2.x, scale2.y), scale2.z)), sc1.radius * (Mathf.Max(Mathf.Max(scale1.x, scale1.y), scale1.z)));
                    descRoundCone.length = (tr2.position - tr1.position).magnitude;

                    solidBehaviour.phSolid.AddShape(phSdk.CreateShape(CDRoundConeIf.GetIfInfoStatic(), descRoundCone));
                    double dposx = descRoundCone.length * Mathf.Sin(tr1.rotation.x * Mathf.Deg2Rad);
                    double dposy = descRoundCone.length * Mathf.Cos(tr1.rotation.x * Mathf.Deg2Rad) * Mathf.Cos(tr1.rotation.z * Mathf.Deg2Rad);
                    double dposz = descRoundCone.length * Mathf.Sin(tr1.rotation.z * Mathf.Deg2Rad);
                    Vec3d p = new Vec3d(dposx / 2, dposy / 2, dposz / 2);
                    Quaterniond q = Quaterniond.Rot(90.0 * Mathf.Deg2Rad, new Vec3d(1.0, 0.0, 0.0));
                    Quaterniond qx = Quaterniond.Rot(-tr1.rotation.x * Mathf.Deg2Rad, new Vec3d(1.0, 0.0, 0.0));
                    Quaterniond qy = Quaterniond.Rot(-tr1.rotation.y * Mathf.Deg2Rad, new Vec3d(0.0, 1.0, 0.0));
                    Quaterniond qz = Quaterniond.Rot(-tr1.rotation.z * Mathf.Deg2Rad, new Vec3d(0.0, 0.0, 1.0));
                    q = q * qx * qy * qz;
                    solidBehaviour.phSolid.SetShapePose(solidBehaviour.phSolid.NShape() - 1, new Posed(q.w, q.x, q.y, q.z, p.x, p.y, p.z));
                }
            }
        }
    }

    public void OnValidate() {
    }
}