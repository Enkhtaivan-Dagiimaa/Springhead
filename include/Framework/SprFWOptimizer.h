#ifndef SPR_FWOPTIMIZER_H
#define SPR_FWOPTIMIZER_H

#include <Springhead.h>
#ifdef USE_CLOSED_SRC
#include "../../closed/include/cmaes/cmaes.h"
#endif

#include<chrono>

namespace Spr{;

/*
  �����V�~�����[�V�������g�����œK���v�Z�̊�{�N���X
  FWApp�̂悤�ɂ��̃N���X���p�����ēƎ��̍œK���A�v���P�[�V�������쐬���邱�Ƃ�z�肷��  


*/

struct FWJointSpecInfo{
public:
	double k_1;
	double k_2;
	double k_3;
	double k_4;
	double min;
	double max;
	bool eulerDir = false;
	FWJointSpecInfo(){
		k_1 = 0.0;
		k_2 = 0.0;
		k_3 = 0.0;
		k_4 = 0.0;
		min = -M_PI;
		max = M_PI;
	}
	FWJointSpecInfo(double a, double b, double c, double d){
		k_1 = a;
		k_2 = b;
		k_3 = c;
		k_4 = d;
		min = -M_PI;
		max = M_PI;
	}
	FWJointSpecInfo(double a, double b, double c, double d, int mi, int ma){
		k_1 = a;
		k_2 = b;
		k_3 = c;
		k_4 = d;
		min = Rad((double)mi);
		max = Rad((double)ma);
	}
	FWJointSpecInfo(double a, double b, double c, double d, int mi, int ma, bool e){
		k_1 = a;
		k_2 = b;
		k_3 = c;
		k_4 = d;
		min = Rad((double)mi);
		max = Rad((double)ma);
		eulerDir = e;
	}
	double Add(double a){
		DSTR << k_1 << " " << k_2 << std::endl;
		return a + 10;
	}

	double Calc(double d){
		return exp(k_1 * (d - k_2)) - exp(k_3 * (k_4 - d));
	}
};

class FW3DJointSpecInfo{
public:
	FWJointSpecInfo specs[3];
	double weight;
	bool lr;
public:
	FW3DJointSpecInfo(){ specs[0] = FWJointSpecInfo(); specs[1] = FWJointSpecInfo(); specs[2] = FWJointSpecInfo(); weight = 0; lr = false; }
	FW3DJointSpecInfo(FWJointSpecInfo zs, double w){ specs[0] = FWJointSpecInfo(); specs[1] = FWJointSpecInfo(); specs[2] = zs, weight = w; lr = false; }
	FW3DJointSpecInfo(FWJointSpecInfo xs, FWJointSpecInfo ys, FWJointSpecInfo zs, double w){ specs[0] = xs; specs[1] = ys; specs[2] = zs, weight = w; lr = false; }
	/*FW3DJointSpecInfo(FWJointSpecInfo s[], double w, bool r){
		int l = sizeof(s[0]) == 0 ? 0 : sizeof(s) / sizeof(s[0]);
		switch (l){
		case 1:
			zspec = s[0];
			break;
		case 3:
			xspec = s[0];
			yspec = s[1];
			zspec = s[2];
			break;
		default:
			break;
		}
		weight = w;
		lr = l;
	}*/
};

struct HingeJointSpecInfo{
	FWJointSpecInfo spec;
	double weight;
	bool lr;
};

class FWConstraint{
protected:
	PHSolidIf* cSolid;
	Posed initialPose;
	double cWeight;
public:
	void SetSolid(PHSolidIf* solid) { this->cSolid = solid; }
	PHSolidIf* GetSolid() { return this->cSolid; }
	void SetInitialPose(Posed pose){ this->initialPose = pose; }
	Posed GetInitialPose(){ return this->initialPose; }
	void SetWeight(double w){ this->cWeight = w; }
	double GetWeight(){ return this->cWeight; }
	virtual double CalcEvalFunc(){ return 0; }
	void Init(){
		if (cSolid != NULL){
			this->initialPose = cSolid->GetPose();
		}
	}
};

class FWGroundConstraint{
private:
	Vec3d cNormal;
	PHSolidIf* cSolid;
	Posed initialPose;
	double cWeight;
public:
	FWGroundConstraint(){ this->cWeight = 1.0; this->cNormal = Vec3d(0, 0, 0); }
	FWGroundConstraint(double w, Vec3d n){ this->cWeight = w;  this->cNormal = n.unit(); }
	void SetSolid(PHSolidIf* solid) { this->cSolid = solid; }
	PHSolidIf* GetSolid() { return this->cSolid; }
	void SetInitialPose(Posed pose){ this->initialPose = pose; }
	Posed GetInitialPose(){ return this->initialPose; }
	void SetWeight(double w){ this->cWeight = w; }
	double GetWeight(){ return this->cWeight; }
	void SetNormal(Vec3d n){ this->cNormal = n.unit(); }
	double CalcEvalFunc(){ 
		Vec3d dPosition = cSolid->GetPose().Pos() - initialPose.Pos();
		Quaterniond dQuaternion = initialPose.Ori().Inv() * cSolid->GetPose().Ori();
		Vec3d n1 = dQuaternion * cNormal;
		double gq = 1.0 - n1 * cNormal;
		double gp = abs(dPosition * cNormal);
		DSTR << "rotationValue:" << gq << " positionValue:" << gp << std::endl;
		return cWeight * (gq + gp);
	}
	void Init(){
		if (cSolid != NULL){
			this->initialPose = cSolid->GetPose();
		}
	}
};

class FWUngroundedConstraint{
private:
	Vec3i cAxis;
	PHSolidIf* cSolid;
	Posed initialPose;
	double cWeight;
public:
	FWUngroundedConstraint(){ this->cWeight = 1.0; this->cAxis = Vec3i(0, 0, 0); }
	FWUngroundedConstraint(double w, Vec3i a){ this->cWeight = w; this->cAxis = a; }
	void SetSolid(PHSolidIf* solid) { this->cSolid = solid; }
	PHSolidIf* GetSolid() { return this->cSolid; }
	void SetInitialPose(Posed pose){ this->initialPose = pose; }
	Posed GetInitialPose(){ return this->initialPose; }
	void SetWeight(double w){ this->cWeight = w; }
	double GetWeight(){ return this->cWeight; }
	void SetAxis(Vec3i a){ this->cAxis = a; }
	double CalcEvalFunc(){
		Vec3d dPosition = cSolid->GetPose().Pos() - initialPose.Pos();
		return cWeight * abs(dPosition * cAxis);
	}
	void Init(){
		if (cSolid != NULL){
			this->initialPose = cSolid->GetPose();
		}
	}
};

struct JointPos {
	Quaterniond ori;
	double angle;
	JointPos() { ori = Quaterniond(); angle = 0; }
	JointPos(Quaterniond q){ ori = q; }
	JointPos(double a){ angle = a; }
};

double ccw(Vec3f p1, Vec3f p2, Vec3f p3);
/*
void GrahamConvexHull(std::vector<Vec3f>& vertices);
bool InsideGrahamConvexHull(std::vector<Vec3f>& vertices, Vec3f gp);
double EvalCenterOfGravity(std::vector<Vec3f>& vertices, Vec3f gp);
*/
class GrahamConvexHull : public UTRefCount{
	struct Sort{
		static float getPolar(Vec3f v1, Vec3f v2){
			Vec3f dv = v2 - v1;
			return dv.norm() == 0 ? 0 : -dv.x / dv.norm();
		}
		struct tang{
			double slope;
			bool same_x = false;
		};
		static tang getTang(Vec3f v1, Vec3f v2){
			tang t;
			if (v1.x == v2.x){
				t.slope = 0;
				t.same_x = true;
			}
			else{
				t.slope = (v1.z - v2.z) / (v1.x - v2.x);
			}
			return t;
		}
		static int part(std::vector<Vec3f> ver, int i, int j, int p){
			int l = i, r = j;

			while (l <= r){
				while (l <= j && ccw(ver[0], ver[l], ver[p]) < 0) l++;
				while (i <= r && ccw(ver[0], ver[r], ver[p]) > 0) r--;
				if (l >= r) break;
				Vec3f v = ver[l];
				ver[l] = ver[r];
				ver[r] = v;
				l++;
				r--;
			}
			return l;
		}
		static void quicksort(std::vector<Vec3f>& ver, int i, int j){
			//DSTR << i << " " << j << std::endl;
			if (i >= j) return; //DSTR << "side error" << std::endl;
			int p = (i + j) / 2;
			int k = part(ver, i, j, p);
			//DSTR << "p:" << p << " k:" << k << std::endl;
			int l = i, r = j;
			while (l <= r){
				while (l <= j && ccw(ver[0], ver[l], ver[p]) < 0) l++;
				while (i <= r && ccw(ver[0], ver[r], ver[p]) > 0) r--;
				if (l >= r) break;
				Vec3f v = ver[l];
				ver[l] = ver[r];
				ver[r] = v;
				//DSTR << "swap(" << l << "," << r << ")" << std::endl;
				l++;
				r--;
			}
			quicksort(ver, i, l - 1);
			quicksort(ver, r + 1, j);
		}
	};
public:
	std::vector<Vec3f> hull;
	Vec3f center;
	Vec3d normal;
public:
	GrahamConvexHull();
	//GrahamConvexHull(std::vector<Vec3f> vertices);
	~GrahamConvexHull();
	void Recalc(std::vector<Vec3f> vertices);
	bool InsideGrahamConvexHull(Vec3f v);
	double Eval(Vec3f v);
	Vec3f* GetVertices();
};

struct FWObjectiveValues{
	double errorvalue = 0;
	double torquevalue = 0;
	double stabilityvalue = 0;
	double groundvalue = 0;
	double ungroundedvalue = 0;
	double centervalue = 0;
	double initialorivalue = 0;
};

struct TorqueResult{
	Vec3d torque;
	Vec3d resist;
	TorqueResult(){ torque = Vec3d(); resist = Vec3d(); }
	TorqueResult(Vec3d t, Vec3d r){ torque = t; resist = r; }
};

class MinJerkTrajectory{
private:
	Posed sPose;
	Posed fPose;
	int stime;
	int ftime;
	//Vec6d coeff[3];
public:
	//�R���X�g���N�^(�������̌W�������߂�)
	//default
	MinJerkTrajectory();
	//�J�n�ʒu�ƏI�[�ʒu�̂�
	MinJerkTrajectory(Posed spose, Posed fpose, int time);
	//�J�n�ƏI�[�̈ʒu�A���x�A�����x�w��
	//MinJerkTrajectory(Posed spose, Posed fpose, Vec6d sVel, Vec6d fVel, Vec6d sAcc, Vec6d fAcc, int time);
	//�J�n�ƏI�_�̈ʒu�A���x�ƒʉߓ_�̈ʒu�A�ʉߎ��Ԏw��
	//MinJerkTrajectory(Posed spose, Posed fpose, Vec6d sVel, Vec6d fVel, Posed vPose, int vtime, int time);
	Posed GetCurrentPose(int t);
	Posed GetDeltaPose(int t);
	double GetCurrentActiveness(int t);
};

class FWOptimizer : public UTRefCount {
protected:
	static FWOptimizer*		instance;
	UTRef<PHSceneIf>		phScene;
	UTRef<UTTimerIf>		thread;
	UTRef<ObjectStatesIf>	states;
	bool					bRunning;
	bool					bInitialized;

	int						dimension;
	double					*arFunvals, *xfinal, *xstart, *stddev, *xprovisional;
	double					*const*pop;
	double                  ixstart = 0.3, istddev = 0.3;

#ifdef USE_CLOSED_SRC
	Parameters<double>		*parameters;
	CMAES<double>			*evo;
	double                  iTolFun = 10, ilambda = 30, iMaxIter = 500;
#endif

	static void SPR_CDECL FWOptimizer_ThreadCallback(int id, void* arg);

public:
	/// Constructor
	FWOptimizer();

	/// Destructor
	~FWOptimizer();

	/// Copy PHScene from passed scene
	void CopyScene(PHSceneIf* phSceneInput);

	/// Get FWScene
	PHSceneIf* GetScene() { return phScene; }

	/// Initialize Optimizer
	void Init(int dimension);

	/// Start Optimization
	void Start();

	/// Abort Optimization
	void Abort();

	/// Check if Running
	bool IsRunning();

	/// Optimization Thread
	void Optimize();

	/// Optimization Iteration Step
	void Iterate();

	/// Apply Poplulation to Scene
	virtual double ApplyPop(PHSceneIf* phScene, double const *x, int n);

	/// Objective Function to Minimize
	virtual double Objective(double const *x, int n);

	/// Return Dimension of Result
	int NResults();

	/// Return Result
	double GetResult(int i);

	/// Return Results
	double* GetResults();

	/// Return Provisional Results
	double* GetProvisionalResults();

	void SetESParameters(double xs, double st, double tf, double la, double mi);
};

class FWStaticTorqueOptimizer : public FWOptimizer {
	std::vector<JointPos> initialPos;
	Vec3d initialRootPos;

	double errorWeight, stabilityWeight, torqueWeight, resistWeight, constWeight, gravcenterWeight, differentialWeight;    //�e�]���lWeight

	std::vector<FWJointSpecInfo> resist;     //joint�̊e�������̒�R�����W��(Ball�͂R�v�f�AHinge�͂P�v�f�g��)�A�����͉��ǂ��܂�
	std::vector<FW3DJointSpecInfo> jointspecs;

	std::vector<JointPos> unityInitPos;      //Unity�̏����p��(�e�֐߂�Euler�p)

	std::vector<double> jointWeights;        //�֐߂��Ƃ�Weight�A�����͊֐ߏ��Ƃ���resist���X�܂Ƃ߂����

	bool specificFlag = false;             //����̊֐߂݂̂Ƀg���N�v�Z��K�p����t���O
	std::vector<int> specificJoints;        //��t���O�L�����̓K�p�֐ߔԍ�

	std::vector<FWGroundConstraint> groundConst;               //�ڒn�S���W���A�Ƃ肠���������₷�������̂�vector
	std::vector<FWUngroundedConstraint> ungroundedConst;       //��ڒn�S��

	//int* bodyIndex;
	std::vector<int> bodyIndex;            //�g�̍\�����̂�index

	//�f�[�^���M�p
	FWObjectiveValues val;                 //�]���l����ۑ��\����
	TorqueResult* tor;                     //�g���N�v�Z���ʕۑ��\���̏W��
	Vec3d cog;                             //�d�S���W
	std::vector<Vec3f> supportPolygon;     //�x�����p�`�`�����_�W��

public:
	virtual void Init();

	virtual void Iterate() { FWOptimizer::Iterate(); }

	virtual void ApplyResult(PHSceneIf* phScene) { FWOptimizer::ApplyPop(phScene, GetResults(), NResults()); }

	virtual double ApplyPop(PHSceneIf* phScene, double const *x, int n);

	virtual double Objective(double const *x, int n);

	void SetScene(PHSceneIf* phSceneInput) { phScene = phSceneInput; }

	void Optimize() { FWOptimizer::Optimize(); }

	bool TestForTermination() {
#ifdef USE_CLOSED_SRC
		return evo->testForTermination();
#else
		return true;
#endif
	}

	void TakeFinalValue() {
#ifdef USE_CLOSED_SRC
		xfinal = evo->getNew(CMAES<double>::XMean);
		Objective(xfinal, (int)evo->get(CMAES<double>::Dimension));
#endif
	}

	Vec3d CalcTorqueN(int n);

	void SetErrorWeight(double v) { errorWeight = v; }
	double GetErrorWeight() { return errorWeight; }

	void SetStabilityWeight(double v) { stabilityWeight = v; }
	double GetStabilityWeight() { return stabilityWeight; }

	void SetTorqueWeight(double v) { torqueWeight = v; }
	double GetTorqueWeight() { return torqueWeight; }

	void SetResistWeight(double v){ resistWeight = v; }
	double GetResistWeight(){ return resistWeight; }

	void SetConstWeight(double v){ constWeight = v; }
	double GetConstWeight(){ return constWeight; }

	void SetGravcenterWeight(double v){ gravcenterWeight = v; }
	double GetGravcenterWeight(){ return gravcenterWeight; }

	void SetDifferentialWeight(double v){ differentialWeight = v; }
	double GetDifferentialWeight(){ return differentialWeight; }
	
	void SetResistanceTorqueN(FWJointSpecInfo h){
		resist.push_back(h);
	}

	void SetResistanceTorque(FWJointSpecInfo h[]){
		int l = sizeof(h) / sizeof(h[0]);
		for (int i = 0; i < l; i++){
			resist.push_back(h[i]);
		}
	}
	
	void SetResistanceTorque(double* h){
		int l = sizeof(h) / (4 * sizeof(h[0]));
		int k = 0;
		for (int i = 0; i < l; i++){
			resist[i].k_1 = h[4*i];
			resist[i].k_2 = h[4 * i + 1];
			resist[i].k_3 = h[4 * i + 2];
			resist[i].k_4 = h[4 * i + 3];
		}
	}

	void AddJointSpecs(FW3DJointSpecInfo j){
		jointspecs.push_back(j);
	}

	void SetUnityInitPos(JointPos j){
		unityInitPos.push_back(j);
	}

	void SetSpecificFlag(bool f){
		this->specificFlag = f;
	}

	void AddSpecificJoints(int n){
		specificJoints.push_back(n);
	}

	void SetSpecificJoints(int n[]){
		int l = sizeof(n) / sizeof(n[0]);
		for (int i = 0; i < l; i++){
			specificJoints.push_back(n[i]);
		}
	}

	/*
	void AddPositionConst(FWConstraint f){
		positionConst.push_back(f);
	}
	*/

	//�\���̂̔z����O��������Ȃ��̂łP�v�f����push
	void AddPositionConst(FWGroundConstraint f){
		groundConst.push_back(f);
	}
	void AddPositionConst(FWUngroundedConstraint f){
		ungroundedConst.push_back(f);
	}

	void SetBodyIndex(int s[]){
		int l = sizeof(s) / sizeof(s[0]);
		for (int i = 0; i < l; i++){
			bodyIndex.push_back(s[i]);
		}
	}
	
	void AddBodyIndex(int s){
	    bodyIndex.push_back(s);
	}

	/*void SetBodyIndex(int s[]){
		int l = sizeof(s) / sizeof(s[0]);
		bodyIndex = new int[l];
		DSTR << l << " " << sizeof(s) << " " << sizeof(s[0]) << std::endl;
		for (int i = 0; i < l; i++){
			bodyIndex[i] = s[i];
		}
	}

	void SetBodyIndex(int s[], int l){
		bodyIndex = new int[l];
		DSTR << l << " " << sizeof(s) << " " << sizeof(s[0]) << std::endl;
		for (int i = 0; i < l; i++){
			bodyIndex[i] = s[i];
		}
	}*/

	void SetESParameters(double xs, double st, double tf, double la, double mi){ FWOptimizer::SetESParameters(xs, st, tf, la, mi); }

	void AddJointWeights(double w){
		jointWeights.push_back(w);
	}

	FWObjectiveValues GetObjectiveValues(){
		return this->val;
	}
	
	int NTorque(){
		//return sizeof(tor[0]) == 0 ? 0 : sizeof(tor) / sizeof(tor[0]);   //new�ŗ̈�m�ۂ����̂ł���ł�sizeof(tor)���@�\���Ȃ�
		return phScene->NIKActuators();
	}

	TorqueResult GetTorqueN(int n){
		return tor[n];
	}
	
	Vec3f GetCenterOfGravity(){
		return cog;
	}

	int NSupportPolygonVertices(){
		return  supportPolygon.size();
	}

	Vec3f GetSupportPolygonVerticesN(int n){
		return supportPolygon[n];
	}
};

class TrajectoryPlanner : public UTRefCount{
	struct LPF{
		//BiQuad LPF
		//�r�Ԃ�
		static PTM::VMatrixRow<double> BiQuad(PTM::VMatrixRow<double> input, double samplerate, double freq, double q){
			PTM::VMatrixRow<double> output;
			//DSTR << input.height() << " " << input.width() << std::endl;
			output.resize(input.height(), input.width());
			output.clear();

			double omega = 2.0 * M_PI * freq / samplerate;
			double alpha = sin(omega) / (2.0 * q);

			double a0 = 1.0 + alpha;
			double a1 = -2.0 * cos(omega);
			double a2 = 1.0 - alpha;
			double b0 = (1.0 - cos(omega)) / 2.0;
			double b1 = 1.0 - cos(omega);
			double b2 = (1.0 - cos(omega)) / 2.0;

			for (int i = 0; i < input.height(); i++){
				output[i][0] = b0 / a0 * input[i][0];
				output[i][1] = b0 / a0 * input[i][1] + b1 / a0 * input[i][0] - a1 / a0 * output[i][0];
				for (int j = 0; j < input.width(); j++){
					output[i][j] = b0 / a0 * input[i][j] + b1 / a0 * input[i][j - 1] + b2 / a0 * input[i][j - 2]
						- a1 / a0 * output[i][j - 1] - a2 / a0 * output[i][j - 2];
				}
			}
			return output;
		}
		//N-Simple Moving Average LPF 
		//�O���t��������ǂ������܂������ĂȂ����ۂ�
		//�ʑ����ꂪ�N���Ă���H
		static PTM::VMatrixRow<double> NSMA(PTM::VMatrixRow<double> input, int n, double mag, PTM::VVector<double> s){
			PTM::VMatrixRow<double> output;
			//DSTR << input.height() << " " << input.width() << std::endl;
			output.resize(input.height(), input.width());
			output.clear();
			DSTR << n << std::endl;

			if (input.width() < n){
				return input;
			}
			else{
				for (int i = 0; i < input.height(); i++){
					int j = 0;
					double sum = s[i] * n;
					DSTR << s[i] << std::endl;
					for (; j < n; j++){
						sum = sum + input[i][j] - s[i];
						output[i][j] = sum / n;
					}
					for (; j < input.width(); j++){
						sum = sum + input[i][j] - input[i][j - n];   //���̂������Ɨݐό덷�o�܂�
						/* �ݐό덷�`�F�b�N�p
						double check_sum = 0;
						for (int k = 0; k < n; k++){
							check_sum += input[i][j - k];
						}
						DSTR << sum - check_sum << std::endl;
						*/
						output[i][j] = mag * sum / n;
					}
				}
			}
			return output;
		}
	};
private:
	//����ΏۂƂȂ�G���h�G�t�F�N�^
	PHIKEndEffectorIf* eef;
	//�J�n�p��
	Posed startPose;
	//�ڕW�p��
	Posed targetPose;
	//�ړ�����
	double mtime;
	//�ړ����Ԃ�Step���ɂ�������(mtime�v��Ȃ��H)
	int movtime;
	//�l������֐߂̐[��
	int depth;
	//�֐߃��[�^��DOF���v
	int motorDOF;
	//�J��Ԃ���
	int iterate;
	//�v�Z�J�n����Step��
	int startStep;
	//Scene�ƕۑ��p��States
	PHSceneIf* scene;
	UTRef<ObjectStatesIf> states;

	//sTorque, torque��CalcTrajectory���ɂ��������g��Ȃ��̂ł�����Ɉړ����ׂ�
	//�O�����瓾���g���N�f�[�^
	PTM::VVector<double> sTorque;
	PTM::VMatrixRow<double> torque;

	//�֐ߊp���l������ꍇ�̃��[�p�X��̊e�֐ߊp
	struct TrajJoint{
		PTM::VMatrixRow<JointPos> data;
		TrajJoint(){ data.clear(); }
		TrajJoint(int depth, int time){ 
			data.resize(depth, time); 
			for (int i = 0; i < time; i++){
				for (int j = 0; j < depth; j++){
					data[j][i] = JointPos();
				}
			}
		}
	};
	std::vector<TrajJoint> trajJointData;
	//std::vector<std::vector<Posed>>��std::vector<PTM::VMatrixRow<double>>��syntax error in input(3)
	//�g���N���琶�������O���f�[�^
	PTM::VMatrixRow<Posed> trajData;
	//�����Ă��邩
	bool moving;
	//(replay����)�Đ��O��index�A�ʏ�͍Ō�̂���
	int ite;
	bool calced;
	//BiQuadLPS�p�̎Ւf���g����Q�l
	double freq;
	double qvalue;
	//N�_����
	int nsma;
	double mag;
	//�g���N->�O����������spring&damper
	double spring;
	double damper;

	//�ȉ�Unity����ǂ��܂œK�p���邩�̃t���O
	//correction��K�p���邩�̃t���O
	bool corFlag;
	//�g���N->�O�Ղ̂Ƃ���IK�ɂ�鐧������邩�̃t���O
	bool IKFlag;
	//�֐߂�pullback��L���ɂ��邩
	bool jointCorFlag;
	//���B�܂ő҂�
	bool waitFlag;
	//�g���N�ω�
	PTM::VVector<double> torquechange;
	double totalchange;
	int best;
	double besttorque;

	//PTM::VMatrixRow<double> torquechangeAll;
	//PTM::VVector<double> totalchangeAll;
	
	bool ignoreMotors;
public:
	//�R���X�g���N�^
	TrajectoryPlanner();
	TrajectoryPlanner(int d, int i, double f, double q, int n, double mg, bool c, bool ik, bool jc, bool im, bool wf);
	//�������n
	void Init();
	void Init(int d, int i, double f, double q, int n, double mg, bool c, bool ik, bool jc, bool im, bool wf);
	//�G���h�G�t�F�N�^�ݒ�
	void SetControlTarget(PHIKEndEffectorIf* e) { this->eef = e; };
	//�V�[���ݒ�
	void SetScene(PHSceneIf* s){ this->scene = s; };
	//�O���v�Z
	void CalcTrajectory(Posed tPose, double mt, int LPFmode);
	//�������ꂽ�O�������ۓK�p
	void TrajStep(bool step);
	//�␳
	void TrajCorrection(int k);
	//
	bool Moving(){ return moving; };
	//�f�o�b�O�p���\��
	void Debug();
	//���[�h
	void LoadScene();
	//spring, damper set
	void SetSD(double s, double d){ this->spring = s; this->damper = d; };
	//replay
	void Replay(int ite);
	//return totalChange
	double GetTotalChange(){ return totalchange; }
	//return best
	int GetBest(){ return best; }
};

}

#endif //SPR_FWOPTIMIZER_H

