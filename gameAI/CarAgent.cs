using System.Collections.Generic;
using UnityEngine;
using System;
using System.Linq;
using System.IO;
using System.Text;

public class CarAgent : Agent
{
    private Sensor[] Sensors { get; set; }
    private RCC_CarControllerV3 Controller { get; set; }
    private Rigidbody CarRb { get; set; }
    private Vector3 StartPosition { get; set; }
    private Quaternion StartRotation { get; set; }
    private Quaternion StartChassisRotation { get; set; }
    public int CurrentStep;
    public int LocalStep;
    private Vector3 LastPosition;
    private Vector3 LastTargetPosition;
    private float LocalDistance;
    StreamWriter writer;//エピソード事の走行距離を書き込む
    private int steps=0;//エピソード数

    public float TotalDistance;

    private double DistFromWall;//wallからの距離の和
    private int cnt;//坂道検出センサー1の緩い傾き
    private int cnt2;//坂道検出センサー2の緩い傾き
    private int cnt3;//3の緩い傾き
    private int cnt11;//1の普通の傾き
    private int cnt22;//2
    private int cnt33;//3
    private int c_steep;//1の急な傾き
    private int c2_steep;//2
    private int c3_steep;//3
    private int TotalDistFlag=0;
    private int statesum;//stateの合計
    private int write_num=400;//書き込むエピソード数

    public float take_dist(){return TotalDistance;}

    private void Awake() {
        CarRb = GetComponent<Rigidbody>();
        Controller = GetComponent<RCC_CarControllerV3>();
        Sensors = GetComponentsInChildren<Sensor>();
    }

    public void Start() {
        Encoding enc = Encoding.GetEncoding("Shift_JIS");//encode
        string path = @"C:\Users\denjo\Desktop\GameAI\Distance.txt";//pathを通す
        writer = new StreamWriter(path, false, enc);//ファイルのopen
        StartPosition = transform.position;
        StartRotation = transform.rotation;
        StartChassisRotation = Controller.chassis.transform.rotation;
        CurrentStep = 0;
        LocalStep = 0;
        LastPosition = StartPosition;
        TotalDistance = 0;
        LocalDistance = 0;
        Controller.useFuelConsumption = false;
    }

    public override void AgentReset() {
        transform.position = StartPosition;
        transform.rotation = StartRotation;
        Controller.chassis.transform.rotation = StartChassisRotation;

        Controller.gasInput = 0;
        Controller.steerInput = 0;
        Controller.clutchInput = 0;
        Controller.steerAngle = 0;

        gameObject.SetActive(false);
        gameObject.SetActive(true);

        Controller.StartEngine();
        CurrentStep = 0;
        LocalStep = 0;
        TotalDistance = 0;
        LocalDistance = 0;
        DistFromWall = 0;//reset
        LastPosition = StartPosition;
        SetReward(0);
    }

    public override int GetState() {

        int StateDivide = 3;
        var results = new List<double>();
        var result = 0;
        Array.ForEach(Sensors, sensor => {
            results.AddRange(sensor.Hits());//0-1
        });
        
        double slope1=0.0d, slope2=0.0d, slope3=0.0d, slope4=0.0d, slope5=0.0d, slope6=0.0d;
        for(int i = 0; i < results.Count; i++) {//results:1だと触れてない
            if(i==4){
                slope1=results[i];
                //Debug.Log(results[i]);
                continue;
            }else if(i==6){
                slope2=results[i];
                continue;
            }else if(i==5){
                slope3=results[i];
                continue;
            }else if(i==7){
                slope4=results[i];
                continue;
            }
            var v = Mathf.CeilToInt(Mathf.Lerp(-1, StateDivide - 2, (float)results[i]));
            if(results[i] >= 0.99f) {
                v = StateDivide - 1;
            }
            //Debug.Log(v);
            result += (int)(v * Mathf.Pow(StateDivide, i));
            DistFromWall += results[i];//報酬を足していく
        }

        int n;//state of speed
        if(CarRb.velocity.magnitude < 10) { n = 0; }
        else if(CarRb.velocity.magnitude < 20) { n = 1; }
        else { n = 2; }
        //Debug.Log(CarRb.velocity.magnitude);

        int tmp = (int)System.Math.Pow(StateDivide+1, 4);//pow(state,sensor_num)
        result += (int)tmp * n;//speed_state

        //坂道検出　平行な2本のセンサーの長さの差
        double dis=System.Math.Abs(slope2-slope1), dis2=System.Math.Abs(slope3-slope4);
        //Debug.Log(dis);
        //Debug.Log(dis2);
        int sl=0, sl2=0;
        //stage3用

        if(dis>0.5){//差がある程度ある期間がある程度続いたらそれは坂道である
            cnt++;
            c_steep++;
            //Debug.Log(3);
        }else if(dis>0.3){//0.2
            cnt++;
            c_steep=0;
            //Debug.Log(2);
        }else{
            cnt=0;
            c_steep=0;
        }
        if(dis2>0.5){
            cnt2++;
            c2_steep++;
            //Debug.Log(6);
        }else if(dis2>0.3){
            cnt2++;
            c2_steep=0;
            //Debug.Log(5);
        }else{
            cnt2=0;
            c2_steep=0;
        }

        if(c_steep>5){//15
            sl=2;
            DistFromWall+=0.7;//坂道であれば、壁だと思って低い報酬を受け取っていたはずなのでその分報酬を与える。
        }else if(cnt>5){
            sl=1;
            DistFromWall+=0.5;
        }
        if(c2_steep>5){
            sl2=2;
            DistFromWall+=0.7;
        }else if(cnt2>5){
            sl2=1;
            DistFromWall+=0.5;
        }
        int sl_out=sl*3+sl2;//0~8
        int tmp_2=tmp*3;
        result+=(int)tmp_2*sl_out;   

        /*stage2を初めに走ったコード。現在のコードの汎用性が高い。
        if(dis>0.2){
            cnt++;
            if(cnt>15){
                //DistFromWall += results[1]+results[2];//前方に坂道があったら前本の反応を報酬とみなす
                sl=1;
                Debug.Log(1);
                DistFromWall+=0.5;//0.5であった
            }
        }else{
            cnt=0;
        }
        if(dis2>0.2){
            cnt2++;
            if(cnt2>15){
                //DistFromWall += results[1]+results[2];//前方に坂道があったら前本の反応を報酬とみなす
                sl2=1;
                Debug.Log(2);
                DistFromWall+=0.5;//0.5
            }
        }else{
            cnt2=0;
        }
        int sl_out=sl*2+sl2;//0~2
        double tmp_2 = tmp * 3;
        result+=(int)tmp_2*sl_out;
        */

        int m;//state of drift
        if(Controller.driftingNow) { m = 0; }
        else { m = 1; }
        int tmp_3 = tmp_2 * 9;//pow(状態数, センサーの数)
        result += (int)tmp_3 * m;//
        var list = new List<float>();
        list.Add(result);
        statesum=tmp_3*2;
        //Debug.Log(statesum);

        return result;
    }

    public override List<double> CollectObservations() {
        // センサーの距離をリストに追加する
        var results = new List<double>();
        Array.ForEach(Sensors, sensor => {
            results.AddRange(sensor.Hits());
        });
        int n;
        //n = SearchNeighbor();
        //results.Add((float)n);
        Vector3 local_v = CarRb.transform.InverseTransformDirection(CarRb.velocity);
        results.Add(local_v.x / 5.0f);
        results.Add(local_v.z / 5.0f);
        if(Controller.driftingNow) { n = -1; }
        else { n = 1; }
        results.Add((float)n);
        return results;
    }

    public override double[] ActionNumberToVectorAction(int ActionNumber) {
        var action = new double[3];
        var steering = 0.0d;
        var braking = 0.0d;
        var gasInput = 0.5d;

        if(ActionNumber % 10 == 1) {
            steering = 1.0d;//1d
            gasInput = 0.3d;//additional
        }
        else if(ActionNumber % 10 == 2) {
            steering = -1.0d;//-1d
            gasInput = 0.3d;//additional
        }
        else if(ActionNumber % 10 == 3) {
            steering = 0.5d;//0.5d
            gasInput = 0.5d;
        }
        else if(ActionNumber % 10 == 4) {
            steering = -0.5d;//-0.5d
            gasInput = 0.5d;
        }
        else if(ActionNumber % 10 == 5) {
            //braking = 0.2d;
            gasInput = 0.7d;//1.0d
        }
        /*else if(ActionNumber % 10 == 6) {
            steering = 0.7d;//0.5d
            gasInput = 0.3d;
        }
        else if(ActionNumber % 10 == 7) {
            steering = -0.7d;//-0.5d
            gasInput = 0.3d;
        }
        else if(ActionNumber % 10 == 8) {
            steering = 0.2d;//0.5d
            gasInput = 0.6d;
        }
        else if(ActionNumber % 10 == 9) {
            steering = -0.2d;//-0.5d
            gasInput = 0.6d;
        }*/

        action[0] = steering;//steering
        action[1] = gasInput;//gasinput
        action[2] = braking;//breaking
        /*if(CarRb.velocity.magnitude>22.0){
            action[0]=1.0d;
            action[1]=0.0d;
        }*/
        return action;//action
    }

    public override void AgentAction(double[] vectorAction) {
        //vectorAction (steering, gasInput, braking (all-1~1))
        CurrentStep++;
        LocalStep++;
        Vector3 CurrentPosition = transform.position;

        LocalDistance = (CurrentPosition - LastPosition).magnitude;
        TotalDistance += LocalDistance;

        var action = (int)vectorAction[0];
        float steering;
        float braking;
        float GasInput;
        //Debug.Log(TotalDistance);
        //Debug.Log(DistFromWall);//報酬確認用
        SetReward((float)DistFromWall);//DistFromWall
        //SetReward((float)LocalDistance);
        //if(TotalDistance%10==0) SetReward(TotalDistance);
        
        DistFromWall = 0;
        if(CarRb.velocity.magnitude<3.0){//speedが遅すぎると止まるので罰を与える。
            SetReward(-(float)DistFromWall);//DistFromWall
        }

        steering = Mathf.Clamp((float)vectorAction[0], -1.0f, 1.0f);
        GasInput = Mathf.Clamp((float)vectorAction[1], 0.0f, 1.0f);
        braking = Mathf.Clamp((float)vectorAction[2], 0.0f, 0.0f);

        Controller.steerInput = steering;
        Controller.gasInput = GasInput;
        Controller.brakeInput = braking;

        LastPosition = CurrentPosition;
        //PassCheck();
    }

    public double RectToPolar(Vector3 pos) {
        double angle = Math.Atan(pos.z / pos.x);
        if(pos.x < 0.0f) {
            if(pos.z <= 0.0f) { angle -= Math.PI; }
            else { angle += Math.PI; }
        }
        return angle;
    }

    /// <summary>
    /// 衝突時に呼び出されるコールバック
    /// </summary>
    /// <param name="collision"></param>
    public void OnCollisionEnter(Collision collision) {
        // 例えばtagがwallだったらの判定
        if(collision.gameObject.tag == "wall") {
            steps++;
            var record = string.Format("{0}", TotalDistance);
            if(steps<write_num) writer.WriteLine(record);
            else if(steps==write_num){
                writer.Close();
                Debug.Log("End");
            }
            
            TotalDistFlag = 0;
            SetReward(-7000.0f);//定数の罰を与える
            Done();
        }else if(collision.gameObject.tag=="goal"){
            //Debug.Log("ok");//十分走ったら報酬を与え、エピソードを終える
            SetReward(10000);
            Done();
        }
    }

    public void OnTriggerEnter(Collider trigger)//gemeobjectを追加して、goalテープとしたり、中間チェックポイントとするときの関数。ゴールテープの役割しかさせない。
    {
        if (trigger.gameObject.tag == "flag0"){
            SetReward(10);
        }else if(trigger.gameObject.tag=="flag1"){
            //Debug.Log("nice");
            SetReward(10000);
        }else if(trigger.gameObject.tag=="flag2"){
            //Debug.Log("excellent");
            SetReward(100000);
        }     
    }

    public override void Stop() {
        CarRb.velocity = Vector3.zero;
        CarRb.angularVelocity = Vector3.zero;
        Controller.gasInput = 0;
        Controller.steerInput = 0;
        Controller.clutchInput = 0;
        Controller.steerAngle = 0;
        Controller.velocityAngle = 0;
        Controller.driftAngle = 0;
        Controller.driftingNow = false;

        //CarRb.isKinematic = true; // set to kinematic on first frame to prevent moving 
        Array.ForEach(Controller.allWheelColliders, wc => {
            wc.totalSlip = 0;
            wc.wheelCollider.brakeTorque = 1.0f;
            wc.wheelCollider.steerAngle = 0;
            wc.wheelCollider.motorTorque = 0;
            wc.wheelModel.position = RCC_GetBounds.GetBoundsCenter(wc.transform);
            wc.wheelModel.rotation = Quaternion.identity;
        });
    }
}
