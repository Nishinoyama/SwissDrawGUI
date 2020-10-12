# include <Siv3D.hpp>
# include"SwissSystemTournament.h"

std::string s3dStringToStdString(String str) {
    return str.toUTF8();
}

class ScenesManager {
    enum SceneEnum
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

    SceneEnum nowScene;
    SwissSystemTournament sst;
    Font normalFont;
    Font cautionFont;

    TextEditState contestNameTextEditState;
    TextEditState playerAmountTextEditState;
    TextEditState roundAmoundTextEditState;

    String errStatus;

    String contestName;
    int playerNumber;
    int roundNumber;
    int processStep;

public:

    ScenesManager() : normalFont(18), cautionFont(18,Typeface::Bold),nowScene(InitialScene)
    {
    }

    void draw() 
    {
        ClearPrint();
        Print << errStatus;
        switch ( nowScene )
        {
        case InitialScene:
            if (SimpleGUI::Button(U"�V�K�쐬", Vec2(120, 480), 220 ))
            {
                sceneChange(SetupScene);
            }
            if (SimpleGUI::Button(U"����CSV����쐬", Vec2(120, 420), 220))
            {
                sceneChange(CSVSetupScene);
            }
            if (SimpleGUI::Button(U"���������[�h", Vec2(460, 480), 220 ))
            {
                sceneChange(LoadScene);
            }
            break;
        case SetupScene:
            normalFont(U"�R���e�X�g���i�t�H���_�̖��O�ɂȂ�܂��j").draw(60, 60, Palette::Black);
            SimpleGUI::TextBox(contestNameTextEditState, Vec2(48, 84), 240);
            normalFont(U"�Q���l���i2�`99999�l�j").draw(60, 156, Palette::Black);
            SimpleGUI::TextBox(playerAmountTextEditState, Vec2(48, 180), 120, 5);
            if (SimpleGUI::Button(U"�L�����Z��", Vec2(120, 480), 180)){
                sceneChange(InitialScene);
            }
            if (SimpleGUI::Button(U"�V�K�f�[�^���쐬", Vec2(440, 480), 280)) {
                try {
                    contestName = contestNameTextEditState.text;
                    playerNumber = ParseOr<int32>(playerAmountTextEditState.text, -199999);
                    if (contestName.empty()) {
                        throw Error(U"�R���e�X�g������ł�");
                    }
                    if (playerNumber == -199999) {
                        throw Error(U"�Q���l���͔��p���l����͂��Ă�������");
                    }
                    if (playerNumber <= 2) {
                        throw Error(U"�Q���l����2�l�ȏ�ɂ��Ă�������");
                    }
                    sst = SwissSystemTournament(playerNumber, 0, s3dStringToStdString(contestName));
                    sst.build();
                    sst.MakeJSONData();
                    roundNumber = 0;
                    sceneChange(NameRateInputScene);
                }
                catch (const Error& e) {
                    errStatus = e.what();
                }
            }
            break;
        case CSVSetupScene:
            normalFont(U"����CSV����쐬�F����CSV�t�@�C����{�R���e�X�g��}/list.csv�ɔz�u���ĉ�����").draw(40, 20, Palette::Black);
            normalFont(U"�R���e�X�g���i�t�H���_�̖��O�ɂȂ�܂��j").draw(60, 60, Palette::Black);
            SimpleGUI::TextBox(contestNameTextEditState, Vec2(48, 84), 240);
            if (SimpleGUI::Button(U"�L�����Z��", Vec2(120, 480), 180)) {
                sceneChange(InitialScene);
            }
            if (SimpleGUI::Button(U"�V�K�f�[�^���쐬", Vec2(440, 480), 280)) {
                try {
                    contestName = contestNameTextEditState.text;
                    if (contestName.empty()) {
                        throw Error(U"�R���e�X�g������ł�");
                    }
                    const CSVData csv( contestName + U"/list.csv");

                    if (!csv) // �����ǂݍ��݂Ɏ��s������
                    {
                        throw Error(U"{}/list.csv�͑��݂��܂���"_fmt(contestName));
                    }
                    for (size_t row = 0; row < csv.rows(); row++)
                    {
                        Print << csv[row];
                        if( csv[row].empty() ) throw Error(U"{}�s�ڂ���s�ł�"_fmt(row + 1));
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
            normalFont(U"�R���e�X�g���i�t�H���_�̖��O�j").draw(60, 60, Palette::Black);
            SimpleGUI::TextBox(contestNameTextEditState, Vec2(48, 84), 240);
            normalFont(U"�������ڏI��������i�����t�@�C����0�j").draw(60, 156, Palette::Black);   
            SimpleGUI::TextBox(roundAmoundTextEditState, Vec2(48, 180), 120, 5);
            if (SimpleGUI::Button(U"�L�����Z��", Vec2(120, 480), 180))
            {
                sceneChange(InitialScene);
            }
            if (SimpleGUI::Button(U"�f�[�^�����[�h", Vec2(440, 480), 280))
            {
                try {
                    contestName = contestNameTextEditState.text;
                    roundNumber = ParseOr<int32>(roundAmoundTextEditState.text, -199999);
                    const JSONReader json(U"{}/data_{}.json"_fmt(contestName, roundNumber));
                    if (contestName.empty()) {
                        throw Error(U"�R���e�X�g������ł�");
                    }
                    if (roundNumber == -199999) {
                        throw Error(U"�������͔��p���l����͂��Ă�������");
                    }
                    if (roundNumber < 0 ) {
                        throw Error(U"��������0�ȏ����͂��Ă�������");
                    }
                    if (!json) {
                        throw Error(U"{}/data_{}.json"_fmt(contestName, roundNumber) + U"�͑��݂��܂���");
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
            normalFont(U"{}/data_0.json�̊e�v���C���[��\n���O�E���[�e�B���O������͂��Ă�������"_fmt(contestName) ).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"���͊���", Vec2(440, 480), 280))
            {
                sceneChange(MatchingProcessScene);
            }
            break;
        case MainScene:
            normalFont(U"{}/matching_{}.json�̊e������\n�f�[�^����͂��Ă�������"_fmt(contestName,roundNumber)).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"���ʓ��͊����I", Vec2(440, 480), 280))
            {
                sceneChange(ProcessingScene);
            }
            break;
        case WithdrewScene:
            normalFont(U"�v���C���[�ɒE���E�������������\n{}/data_{}.json�̊Y���v���C���[��withdrew��true�ɓ��͂��Ă�������"_fmt(contestName, roundNumber)).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"���͊���", Vec2(440, 480), 280))
            {
                sceneChange(MatchingProcessScene);
            }
            break;
        case MatchingProcessScene:
            switch (processStep)
            {
            case 0:
                Print << U"�����̂ЂȌ^�𐶐��c";
                break;
            case 1:
                sst = SwissSystemTournament(playerNumber, roundNumber, s3dStringToStdString(contestName));
                Print << U"JSON�t�@�C���ǂݍ��ݒ��c";
                break;
            case 2:
                sst.buildFromJSON();
                Print << U"�}�b�`���O�������c";
                break;
            case 3:
                sst.Matching();
                roundNumber++;
                Print << U"JSON�}�b�`���O�f�[�^�o�́c";
                break;
            case 4:
                sst.OutputMatching();
                sceneChange(MainScene);
                break;
            default:
                Print << U"����I���������������ł��B";
                break;
            }
            processStep++;
            break;
        case ProcessingScene:
            switch (processStep)
            {
            case 0:
                Print << U"���ʓ��͏����c";
                break;
            case 1:
                sst.InputMatchResult();
                Print << U"HTML���ʕ\�o�́c";
                // sst.dropOutByPoint(2 * roundNumber - 5);
                break;
            case 2:
                sst.OutputFinalResult();
                Print << U"JSON�f�[�^�o�́c";
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
                Print << U"����I���������������ł��B";
                break;
            }
            processStep++;
            break;
        case EndScene:
            normalFont(U"�S���̃}�b�`���O���I�����܂����B\n{}/result_{}.html����ŏI���ʕ\���m�F���Ă��������B"_fmt(contestName, roundNumber)).draw(60, 60, Palette::Black);
            if (SimpleGUI::Button(U"�I��", Vec2(440, 480), 280))
            {
                sceneChange(InitialScene);
            }
            break;
        default:
            ClearPrint();
            Print << U"����I���������������ł��B";
            break;
        }
    }

    void sceneChange(SceneEnum s) {
        errStatus = U"";
        nowScene = s;
        processStep = 0;
    }

};


void Main()
{
    Scene::SetBackground(ColorF(0.8, 0.9, 0.7));
    Window::SetTitle(U"�X�C�X���g�[�i�����g�E�}�b�`���O�A�v��");

    ScenesManager sm;

    while (System::Update())
    {
        sm.draw();
    }
}
