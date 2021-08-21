Scriptname AAF:Info Native Hidden Const

; actor types
; non-human type definition in list below means male
; female version of actor type is calculated as actor type + 1

int property M = 1 autoreadonly
int property F = 2 autoreadonly

int property Cat = 5 autoreadonly
int property Dog = 9 autoreadonly
int property Alien = 13 autoreadonly
int property Robot = 17 autoreadonly
int property Swarm = 21 autoreadonly
int property Angler = 25 autoreadonly
int property EyeBot = 29 autoreadonly
int property Gulper = 33 autoreadonly
int property Hermit = 37 autoreadonly
int property Brahmin = 41 autoreadonly
int property Gorilla = 45 autoreadonly
int property Molerat = 49 autoreadonly
int property RadStag = 53 autoreadonly
int property YaoGuai = 57 autoreadonly
int property Bloatfly = 61 autoreadonly
int property FEVHound = 65 autoreadonly
int property Mirelurk = 69 autoreadonly
int property Mosquito = 73 autoreadonly
int property RadRoach = 77 autoreadonly
int property Bloodworm = 81 autoreadonly
int property Deathclaw = 85 autoreadonly
int property Gatorclaw = 89 autoreadonly
int property RadRabbit = 93 autoreadonly
int property Stingwing = 97 autoreadonly
int property CreateABot = 101 autoreadonly
int property FeralGhoul = 105 autoreadonly
int property FogCrawler = 109 autoreadonly
int property PowerArmor = 113 autoreadonly
int property RadChicken = 117 autoreadonly
int property Animatronic = 121 autoreadonly
int property CaveCricket = 125 autoreadonly
int property GulperLarge = 129 autoreadonly
int property RadScorpion = 133 autoreadonly
int property Supermutant = 137 autoreadonly
int property LibertyPrime = 141 autoreadonly
int property MirelurkKing = 145 autoreadonly
int property MirelurkQueen = 149 autoreadonly
int property MirelurkHunter = 153 autoreadonly
int property HandyCreateABot = 157 autoreadonly
int property SupermutantBehemoth = 161 autoreadonly


; scene types

int property Loving = -1 autoreadonly
int property Neutral = 0 autoreadonly
int property Rough = 1 autoreadonly
int property Aggressive = 1 autoreadonly
int property RoughAggressive = 2 autoreadonly


; actor position

int property FromSide = 1 autoreadonly
int property FromFront = 2 autoreadonly
int property FromBehind = 4 autoreadonly

; plugin's version
string function GetVersionString() global native 
; plugin's version
int function GetVersionInt() global native 

Struct SceneInfo
    string Name ; position name
    int Type ; see constants above
    string Narrative ; what animation is about
    string Author ; if known
    string Attributes ; comma separated atrtibutes
    string Service; comma separated service attributes
    string Furn ; Furniture, if recognized
    string Unparsed ; tags that are not recognized. separated by comma
EndStruct

Struct ActorInfo
    Actor Participant
    int Index
    int Type ; actor's type. see constants above

    ; numeric attributes
    int HeldLevel ; [0,9], 0 - free, 9 - max restrained
    int StimLevel ; [-9,9],  negative values - pain
    int LoveLevel ; [-9,9],  negative values - hate
    int DomLevel ; [-9,9], defined for 0 actor only. negative values - submissiveness

    ; flag attributes
    bool IsAggressor
    bool IsVictim

    ; integer value means amout of 'gives' or 'takes'
    ; for example IsHandGiver = 2 means that actor uses 2 hands to contact other actor(s)

    int IsHandGiver
    int IsHandTaker
    int IsLegGiver
    int IsLegTaker
    int IsOralGiver
    int IsOralTaker
    int IsGenitalGiver
    int IsGenitalTaker

    int IsAnalTaker
    int IsButtTaker 
    int IsNippleTaker

    int IsOviposGiver

    ; user defined data, not populated by API
    ; just to avoid necessity to copy this structure content

    float StartTime
    float UserValue1
    int UserValue2
    string UserValue3
EndStruct

; returns actors information for provided AAF position
;  positionName - name of scene, that user could see in AAF UI
;  actors - list of actors participated in scene
; returns None on failure. For example, when position with such name is not exist or list of actors does not corresponds to actors defined for animation
ActorInfo[] function GetActors(string positionName, Actor[] actors) global native

; returns AAF position information
;  positionName - name of scene, that user could see in AAF UI
SceneInfo function GetScene(string positionName) global native

; Populates output with actors information and returns AAF position information
;  positionName - name of scene, that user could see in AAF UI
;  actors - list of actors participated in scene
;  output - must be allocated and initilized by script. if some element is None then it will be skipped
;     if length of output is less than actor's length then then first N actor's information will be populated
SceneInfo function GetSceneAndActors(string positionName, Actor[] actors, ActorInfo[] output) global native