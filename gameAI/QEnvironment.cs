using System;
using UnityEngine;
using UnityEngine.UI;
using System.IO;
using System.Text;
using System.Collections.Generic;

public class QEnvironment : Environment
{
    [SerializeField] private GameObject GObject;

    [SerializeField] private int actionSize = 6;
    private int ActionSize { get { return actionSize; } }

    [SerializeField] private int stateSize = 4;
    private int StateSize { get { return stateSize; } }

    [SerializeField] private bool sarsa = false;

    private QBrain QLBrain;
    private Agent LearningAgent;

    private int PrevState;
    private int PrevActionNo;//for sarsa
    private int episods=0;//episode num
    private float totalreward=0.0f;//for data
    public float totald=0.0f;//for data
    private int write_num = 400;//書き込むエピソード数

    StreamWriter writer;

    void Start()//再生ボタンを押すと一度だけ呼ばれる
    {
        Encoding enc = Encoding.GetEncoding("Shift_JIS");//encode
        string path = @"C:\Users\denjo\Desktop\GameAI\Reward.txt";//pathを通す
        writer = new StreamWriter(path, false, enc);//ファイルのopen
        QLBrain = new QBrain(StateSize, ActionSize);
        LearningAgent = GObject.GetComponent<Agent>();
        PrevState = LearningAgent.GetState();

        PrevActionNo = QLBrain.GetAction(PrevState);//sarsa用
    }

    private void FixedUpdate()//毎ステップ実行される
    {
        if(sarsa) AgentUpdateSarsa(LearningAgent, QLBrain);
        else AgentUpdate(LearningAgent, QLBrain);

        if (LearningAgent.IsDone)//episodeが終わったら実行される
        {
            if(episods<write_num){//ファイル書き込み
                var record = string.Format("{0}", totalreward);
                writer.WriteLine(record);//各エピソードでの合計報酬
            }else if(episods==write_num){
                writer.Close();
                Debug.Log("End");
            }
            episods++;
            totald = 0;
            totalreward = 0;
            LearningAgent.Reset();
        }
    }

    private void AgentUpdate(Agent a, QBrain b)//q学習
    {
        int ActionNo = b.GetAction(PrevState);
        var action = a.ActionNumberToVectorAction(ActionNo);
        a.AgentAction(action);//action
        var NewState = a.GetState();//走った後のステート
        b.UpdateTable(PrevState, NewState, ActionNo, a.Reward, a.IsDone, ref totalreward);
        PrevState = NewState;
    }

    private void AgentUpdateSarsa(Agent a, QBrain b)//sarsa:flagを立てるとこちらで実行可能
    {
        var prevaction = a.ActionNumberToVectorAction(PrevActionNo);
        a.AgentAction(prevaction);
        var NewState = a.GetState();
        int NewActionNo = b.GetAction(NewState);
        b.UpdateTableSarsa(PrevState, NewState, PrevActionNo, a.Reward, a.IsDone, NewActionNo, ref totalreward);
        PrevState = NewState;
        PrevActionNo = NewActionNo;
    }
}
