#include <string>
#include <unordered_map>

#include "Util.hpp"
#include "ClassData.hpp"

std::unordered_map<std::string, ClassResources> ClassData::nameToClassResources;

ClassResources::ClassResources() {}
ClassResources::ClassResources(std::string internalName, ClassType type)
{
    classType = type;

    std::string filePath = "res/Classes/" + internalName + ".class";

    std::vector<std::string> file = Util::readFile(filePath);
    displayName            = file[ 1];
    mapSpritesPath         = file[ 4];
    battleSpritesPath      = file[ 7];
    genericMugshotPath     = file[10];
    genericMugshotTinyPath = file[13];

    capHp  = stoi(Util::split(file[16], ' ')[1]);
    capStr = stoi(Util::split(file[17], ' ')[1]);
    capMag = stoi(Util::split(file[18], ' ')[1]);
    capSkl = stoi(Util::split(file[19], ' ')[1]);
    capSpd = stoi(Util::split(file[20], ' ')[1]);
    capLck = stoi(Util::split(file[21], ' ')[1]);
    capDef = stoi(Util::split(file[22], ' ')[1]);
    capRes = stoi(Util::split(file[23], ' ')[1]);
    capCon = stoi(Util::split(file[24], ' ')[1]);
    capMov = stoi(Util::split(file[25], ' ')[1]);

    promoHp  = stoi(Util::split(file[28], ' ')[1]);
    promoStr = stoi(Util::split(file[29], ' ')[1]);
    promoMag = stoi(Util::split(file[30], ' ')[1]);
    promoSkl = stoi(Util::split(file[31], ' ')[1]);
    promoSpd = stoi(Util::split(file[32], ' ')[1]);
    promoLck = stoi(Util::split(file[33], ' ')[1]);
    promoDef = stoi(Util::split(file[34], ' ')[1]);
    promoRes = stoi(Util::split(file[35], ' ')[1]);
    promoCon = stoi(Util::split(file[36], ' ')[1]);
    promoMov = stoi(Util::split(file[37], ' ')[1]);

    promoSword = stoi(Util::split(file[40], ' ')[1]);
    promoLance = stoi(Util::split(file[41], ' ')[1]);
    promoAxe   = stoi(Util::split(file[42], ' ')[1]);
    promoBow   = stoi(Util::split(file[43], ' ')[1]);
    promoAnima = stoi(Util::split(file[44], ' ')[1]);
    promoLight = stoi(Util::split(file[45], ' ')[1]);
    promoDark  = stoi(Util::split(file[46], ' ')[1]);
    promoStaff = stoi(Util::split(file[47], ' ')[1]);
}

void ClassData::init()
{
    nameToClassResources.insert({"LordCaelin"      , ClassResources("LordCaelin"      , LordCaelin      )});
    nameToClassResources.insert({"BladeLord"       , ClassResources("BladeLord"       , BladeLord       )});
    nameToClassResources.insert({"LordPherae"      , ClassResources("LordPherae"      , LordPherae      )});
    nameToClassResources.insert({"KnightLord"      , ClassResources("KnightLord"      , KnightLord      )});
    nameToClassResources.insert({"LordOstia"       , ClassResources("LordOstia"       , LordOstia       )});
    nameToClassResources.insert({"GreatLord"       , ClassResources("GreatLord"       , GreatLord       )});
    nameToClassResources.insert({"Mercenary"       , ClassResources("Mercenary"       , Mercenary       )});
    nameToClassResources.insert({"HeroF"           , ClassResources("HeroF"           , HeroF           )});
    nameToClassResources.insert({"HeroM"           , ClassResources("HeroM"           , HeroM           )});
    nameToClassResources.insert({"MyrmidonF"       , ClassResources("MyrmidonF"       , MyrmidonF       )});
    nameToClassResources.insert({"MyrmidonM"       , ClassResources("MyrmidonM"       , MyrmidonM       )});
    nameToClassResources.insert({"SwordmasterF"    , ClassResources("SwordmasterF"    , SwordmasterF    )});
    nameToClassResources.insert({"SwordmasterM"    , ClassResources("SwordmasterM"    , SwordmasterM    )});
    nameToClassResources.insert({"ThiefF"          , ClassResources("ThiefF"          , ThiefF          )});
    nameToClassResources.insert({"ThiefM"          , ClassResources("ThiefM"          , ThiefM          )});
    nameToClassResources.insert({"Assassin"        , ClassResources("Assassin"        , Assassin        )});
    nameToClassResources.insert({"Knight"          , ClassResources("Knight"          , Knight          )});
    nameToClassResources.insert({"General"         , ClassResources("General"         , General         )});
    nameToClassResources.insert({"Soldier"         , ClassResources("Soldier"         , Soldier         )});
    nameToClassResources.insert({"Fighter"         , ClassResources("Fighter"         , Fighter         )});
    nameToClassResources.insert({"Warrior"         , ClassResources("Warrior"         , Warrior         )});
    nameToClassResources.insert({"Brigand"         , ClassResources("Brigand"         , Brigand         )});
    nameToClassResources.insert({"Pirate"          , ClassResources("Pirate"          , Pirate          )});
    nameToClassResources.insert({"Corsair"         , ClassResources("Corsair"         , Corsair         )});
    nameToClassResources.insert({"Berserker"       , ClassResources("Berserker"       , Berserker       )});
    nameToClassResources.insert({"ArcherF"         , ClassResources("ArcherF"         , ArcherF         )});
    nameToClassResources.insert({"ArcherM"         , ClassResources("ArcherM"         , ArcherM         )});
    nameToClassResources.insert({"SniperF"         , ClassResources("SniperF"         , SniperF         )});
    nameToClassResources.insert({"SniperM"         , ClassResources("SniperM"         , SniperM         )});
    nameToClassResources.insert({"NomadF"          , ClassResources("NomadF"          , NomadF          )});
    nameToClassResources.insert({"NomadM"          , ClassResources("NomadM"          , NomadM          )});
    nameToClassResources.insert({"NomadTrooperF"   , ClassResources("NomadTrooperF"   , NomadTrooperF   )});
    nameToClassResources.insert({"NomadTrooperM"   , ClassResources("NomadTrooperM"   , NomadTrooperM   )});
    nameToClassResources.insert({"Cavalier"        , ClassResources("Cavalier"        , Cavalier        )});
    nameToClassResources.insert({"Paladin"         , ClassResources("Paladin"         , Paladin         )});
    nameToClassResources.insert({"PegasusKnight"   , ClassResources("PegasusKnight"   , PegasusKnight   )});
    nameToClassResources.insert({"FalconKnight"    , ClassResources("FalconKnight"    , FalconKnight    )});
    nameToClassResources.insert({"WyvernRider"     , ClassResources("WyvernRider"     , WyvernRider     )});
    nameToClassResources.insert({"WyvernLord"      , ClassResources("WyvernLord"      , WyvernLord      )});
    nameToClassResources.insert({"Monk"            , ClassResources("Monk"            , Monk            )});
    nameToClassResources.insert({"Cleric"          , ClassResources("Cleric"          , Cleric          )});
    nameToClassResources.insert({"BishopF"         , ClassResources("BishopF"         , BishopF         )});
    nameToClassResources.insert({"BishopM"         , ClassResources("BishopM"         , BishopM         )});
    nameToClassResources.insert({"Troubadour"      , ClassResources("Troubadour"      , Troubadour      )});
    nameToClassResources.insert({"Valkyrie"        , ClassResources("Valkyrie"        , Valkyrie        )});
    nameToClassResources.insert({"MageF"           , ClassResources("MageF"           , MageF           )});
    nameToClassResources.insert({"MageM"           , ClassResources("MageM"           , MageM           )});
    nameToClassResources.insert({"SageF"           , ClassResources("SageF"           , SageF           )});
    nameToClassResources.insert({"SageM"           , ClassResources("SageM"           , SageM           )});
    nameToClassResources.insert({"Archsage"        , ClassResources("Archsage"        , Archsage        )});
    nameToClassResources.insert({"ShamanF"         , ClassResources("ShamanF"         , ShamanF         )});
    nameToClassResources.insert({"ShamanM"         , ClassResources("ShamanM"         , ShamanM         )});
    nameToClassResources.insert({"DruidF"          , ClassResources("DruidF"          , DruidF          )});
    nameToClassResources.insert({"DruidM"          , ClassResources("DruidM"          , DruidM          )});
    nameToClassResources.insert({"DarkDruid"       , ClassResources("DarkDruid"       , DarkDruid       )});
    nameToClassResources.insert({"Bard"            , ClassResources("Bard"            , Bard            )});
    nameToClassResources.insert({"Dancer"          , ClassResources("Dancer"          , Dancer          )});
    nameToClassResources.insert({"MagicSeal"       , ClassResources("MagicSeal"       , MagicSeal       )});
    nameToClassResources.insert({"Bramimond"       , ClassResources("Bramimond"       , Bramimond       )});
    nameToClassResources.insert({"FireDragon"      , ClassResources("FireDragon"      , FireDragon      )});
    nameToClassResources.insert({"Man"             , ClassResources("Man"             , Man             )});
    nameToClassResources.insert({"Prince"          , ClassResources("Prince"          , Prince          )});
    nameToClassResources.insert({"TransporterTent" , ClassResources("TransporterTent" , TransporterTent )});
    nameToClassResources.insert({"TransporterWagon", ClassResources("TransporterWagon", TransporterWagon)});
}

//std::string ClassData::getClassName(ClassType type)
//{
//    for (auto it = nameToClass.begin(); it != nameToClass.end(); ++it)
//    {
//        if (it->second == type)
//        {
//            return it->first;
//        }
//    } 
//
//    return "Prince";
//}

ClassResources ClassData::getClassResources(std::string name)
{
    auto a = nameToClassResources.find(name);
    if (a != nameToClassResources.end())
    {
        return a->second;
    }

    return nameToClassResources["Brigand"];
}

