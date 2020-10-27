# include <Siv3D.hpp>
# include"SwissSystemTournament.h"

std::string s3dStringToStdString(String str) {
    return str.toUTF8();
}


struct SceneState
{
};


struct SwissData 
{
    /*

    SwissScene* nowScene;

    TextEditState contestNameTextEditState;
    TextEditState playerAmountTextEditState;
    TextEditState roundAmoundTextEditState;

    */

    SwissSystemTournament sst;
    String errStatus;
    String contestName;
    int playerNumber;
    int roundNumber;
    int processStep;

    /*
public:
 
    SwissSceneManager() : normalFont(18), cautionFont(18,Typeface::Bold),nowScene(InitialScene)
    {
        
    }

    void draw() 
    {
        ClearPrint();
        nowScene->Draw();
        Print << errStatus;
        switch ( nowScene )
        {
        case InitialScene:
            
        case SetupScene:
            break;
        case CSVSetupScene:
            normalFont(U"名簿CSVから作成：名簿CSVファイルを{コンテスト名}/list.csvに配置して下さい").draw(40, 20, Palette::Black);
            normalFont(U"コンテスト名（フォルダの名前になります）").draw(60, 60, Palette::Black);
            SimpleGUI::TextBox(contestNameTextEditState, Vec2(48, 84), 240);
            if (SimpleGUI::Button(U"キャンセル", Vec2(120, 480), 180)) {
                sceneChange(InitialScene);
            }
            if (SimpleGUI::Button(U"新規データを作成", Vec2(440, 480), 280)) {
                try {
                    contestName = contestNameTextEditState.text;
                    if (contestName.empty()) {
                        throw Error(U"コンテスト名が空です");
                    }
                    const CSVData csv( contestName + U"/list.csv");

                    if (!csv) // もし読み込みに失敗したら
                    {
                        throw Error(U"{}/list.csvは存在しません"_fmt(contestName));
                    }
                    for (size_t row = 0; row < csv.rows(); row++)
                    {
                        Print << csv[row];
                        if( csv[row].empty() ) throw Error(U"{}行目が空行です"_fmt(row + 1));
                    }
                    playerNumber = csv.rows();
                    sst = SwissSystemTournament(playerNumber, 0, contestName.toUTF8());
                    sst.build();
                    for (size_t row = 0; row < playerNumber; row++)
                    {
                        std::string playerName = s3dStringToStdString(csv[row].at(0));
                        int rating = 0;
                        if (csv[row].size() > 1) rating = ParseOr<int32>(csv[row].at(1), 0);
                        sst.setPlayer(row, playerName, rating);
                    }
                    sst.MakeJSONData();
                    roundNumber = 0;
                    sceneChange(MatchingProcessScene);
                }
                catch (const Error& e) {
                    errStatus = e.what();
                }
            }
            break;
        case LoadScene:
            normalFont(U"コンテスト名（フォルダの名前）").draw(60, 60, Palette::Black);
            SimpleGUI::TextBox(contestNameTextEditState, Vec2(48, 84), 240);
            normalFont(U"何試合目終了時から（初期ファイルは0）").draw(60, 156, Palette::Black);   
            SimpleGUI::TextBox(roundAmoundTextEditState, Vec2(48, 180), 120, 5);
            if (SimpleGUI::Button(U"キャンセル", Vec2(120, 480), 180))
            {
                sceneChange(InitialScene);
            }
            if (SimpleGUI::Button(U"データをロード", Vec2(440, 480), 280))
            {
                try {
                    contestName = contestNameTextEditState.text;
                    roundNumber = ParseOr<int32>(roundAmoundTextEditState.text, -199999);
                    const JSONReader json(U"{}/data_{}.json"_fmt(contestName, roundNumber));
                    if (contestName.empty()) {
                        throw Error(U"コンテスト名が空です");
                    }
                    if (roundNumber == -199999) {
                        throw Error(U"試合数は半角数値を入力してください");
                    }
                    if (roundNumber < 0 ) {
                        throw Error(U"試合数は0以上を入力してください");
                    }
                    if (!json) {
                        throw Error(U"{}/data_{}.json"_fmt(contestName, roundNumber) + U"は存在しません");
                    }
                    sst = SwissSystemTournament(roundNumber, s3dStringToStdString(contestName));
                    const JSONReader matchingJson(U"{}/matching_{}.json"_fmt(contestName, roundNumber));

                    // if (!matchingJson) {
                        sceneChange(MatchingProcessScene);
                    // }
                    // else {
                    //     sceneChange(MainScene);
                    // }
                }
                catch (const Error& e)
                {
                    errStatus = e.what();
                }
            }
            break;
        case NameRateInputScene:
            normalFont(U"{}/data_0.jsonの各プレイヤーの\n名前・レーティング情報を入力してください"_fmt(contestName) ).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"入力完了", Vec2(440, 480), 280))
            {
                sceneChange(MatchingProcessScene);
            }
            break;
        case MainScene:
            normalFont(U"{}/matching_{}.jsonの各試合の\nデータを入力してください"_fmt(contestName,roundNumber)).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"結果入力完了！", Vec2(440, 480), 280))
            {
                sceneChange(ProcessingScene);
            }
            break;
        case WithdrewScene:
            normalFont(U"プレイヤーに脱落・棄権等があれば\n{}/data_{}.jsonの該当プレイヤーのwithdrewをtrueに入力してください"_fmt(contestName, roundNumber)).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"入力完了", Vec2(440, 480), 280))
            {
                sceneChange(MatchingProcessScene);
            }
            break;
        case MatchingProcessScene:
            switch (processStep)
            {
            case 0:
                Print << U"試合のひな型を生成…";
                break;
            case 1:
                sst = SwissSystemTournament(playerNumber, roundNumber, s3dStringToStdString(contestName));
                Print << U"JSONファイル読み込み中…";
                break;
            case 2:
                sst.buildFromJSON();
                Print << U"マッチング生成中…";
                break;
            case 3:
                sst.Matching();
                roundNumber++;
                Print << U"JSONマッチングデータ出力…";
                break;
            case 4:
                sst.OutputMatching();
                sceneChange(MainScene);
                break;
            default:
                Print << U"うわ！何かがおかしいです。";
                break;
            }
            processStep++;
            break;
        case ProcessingScene:
            switch (processStep)
            {
            case 0:
                Print << U"結果入力処理…";
                break;
            case 1:
                sst.InputMatchResult();
                Print << U"HTML順位表出力…";
                // sst.dropOutByPoint(2 * roundNumber - 5);
                break;
            case 2:
                sst.OutputFinalResult();
                Print << U"JSONデータ出力…";
                break;
            case 3:
                if (roundNumber == playerNumber-1+playerNumber%2 )
                {
                    sceneChange(EndScene);
                }
                else
                {
                    sst.MakeJSONData();
                    sceneChange(WithdrewScene);
                }
                break;
            case 4:
                break;
            default:
                Print << U"うわ！何かがおかしいです。";
                break;
            }
            processStep++;
            break;
        case EndScene:
            normalFont(U"全部のマッチングが終了しました。\n{}/result_{}.htmlから最終順位表を確認してください。"_fmt(contestName, roundNumber)).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"終了", Vec2(440, 480), 280))
            {
                sceneChange(InitialScene);
            }
            break;
        default:
            ClearPrint();
            Print << U"うわ！何かがおかしいです。";
            break;
        }
    }

    void sceneChange(SwissScene* s) {
        errStatus = U"";
        nowScene = s;
        processStep = 0;
    }

  */
};
using App = SceneManager<String, SwissData>;
enum class SceneEnum
{
    InitialScene,
    SetupScene,
    CSVSetupScene,
    MainScene,
    ProcessingScene,
    NameRateInputScene,
    MatchingProcessScene,
    LoadScene,
    WithdrewScene,
    EndScene
};

class SwissScene : public App::Scene {
public:
    SwissScene(const InitData& init) : IScene(init) {};
    void update() override {};
    void draw() const override {
        ClearPrint();
        Print << getData().errStatus;
    };
protected:
    bool changeScene(const State_t& state, const Duration& transitionTime = MillisecondsF(  0), bool crossFade = false) {
        getData().errStatus = U"";
        return IScene::changeScene(state, transitionTime, crossFade);
    }
};

class InitialScene : public SwissScene {
public:
    InitialScene(const InitData& init) : SwissScene(init) {};
    void update() override {
        if (SimpleGUI::Button(U"新規作成", Vec2(120, 480), 220))
        {
            changeScene(U"Setup");
        }
        if (SimpleGUI::Button(U"名簿CSVから作成", Vec2(120, 420), 220))
        {
            // sceneChange(CSVSetupScene);
        }
        if (SimpleGUI::Button(U"続きをロード", Vec2(460, 480), 220))
        {
            // sceneChange(LoadScene);
        }
    }
};

class SetupScene : public SwissScene {
private:
    TextEditState contestNameTextEditState;
    TextEditState playerAmountTextEditState;
    TextEditState roundAmoundTextEditState;
public:
    SetupScene(const InitData& init) : SwissScene(init) {};
    void update() override {
        FontAsset(U"NormalFont")(U"コンテスト名（フォルダの名前になります）").draw(60, 60, Palette::Black);
        SimpleGUI::TextBox(contestNameTextEditState, Vec2(48, 84), 240);
        FontAsset(U"NormalFont")(U"参加人数（2～99999人）").draw(60, 156, Palette::Black);
        SimpleGUI::TextBox(playerAmountTextEditState, Vec2(48, 180), 120, 5);
        if (SimpleGUI::Button(U"キャンセル", Vec2(120, 480), 180)) {
            changeScene(U"Initial");
        }
        if (SimpleGUI::Button(U"新規データを作成", Vec2(440, 480), 280)) {
            try {
                const String contestName = contestNameTextEditState.text;
                const int playerNumber = ParseOr<int32>(playerAmountTextEditState.text, -199999);
                if (contestName.empty()) {
                    throw Error(U"コンテスト名が空です");
                }
                if (playerNumber == -199999) {
                    throw Error(U"参加人数は半角数値を入力してください");
                }
                if (playerNumber <= 2) {
                    throw Error(U"参加人数は3人以上にしてください");
                }
                getData().contestName = contestName;
                getData().playerNumber = playerNumber;
                getData().sst = SwissSystemTournament(playerNumber, 0, s3dStringToStdString(contestName));
                getData().sst.build();
                getData().sst.MakeJSONData();
                getData().roundNumber = 0;
                changeScene(U"NameRateInput");
            }
            catch (const Error& e) {
                getData().errStatus = e.what();
            }
        }
    }
};

void Main()
{
    FontAsset::Register(U"NormalFont", 18);
    FontAsset::Register(U"CautionFont", 18, Typeface::Bold);
    Scene::SetBackground(ColorF(0.8, 0.9, 0.7));
    Window::SetTitle(U"スイス式トーナメント・マッチングアプリ");

    App manager;
    manager
        .add<InitialScene>(U"Initial")
        .add<SetupScene>(U"Setup")
        ;

    while (System::Update())
    {
        if (!manager.update()) {
            break;
        }
    }
}
