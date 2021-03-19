#include <string>
#include <unordered_map>

#include "ClassData.hpp"

std::unordered_map<std::string, ClassType> ClassData::nameToClass;

void ClassData::init()
{
    nameToClass.insert({"Lord Caelin"      , LordCaelin      });
    nameToClass.insert({"Blade Lord"       , BladeLord       });
    nameToClass.insert({"Lord Pherae"      , LordPherae      });
    nameToClass.insert({"Knight Lord"      , KnightLord      });
    nameToClass.insert({"Lord Ostia"       , LordOstia       });
    nameToClass.insert({"Great Lord"       , GreatLord       });
    nameToClass.insert({"Mercenary"        , Mercenary       });
    nameToClass.insert({"Hero"             , Hero            });
    nameToClass.insert({"Myrmidon"         , Myrmidon        });
    nameToClass.insert({"Swordmaster"      , Swordmaster     });
    nameToClass.insert({"Thief"            , Thief           });
    nameToClass.insert({"Assassin"         , Assassin        });
    nameToClass.insert({"Knight"           , Knight          });
    nameToClass.insert({"General"          , General         });
    nameToClass.insert({"Soldier"          , Soldier         });
    nameToClass.insert({"Fighter"          , Fighter         });
    nameToClass.insert({"Warrior"          , Warrior         });
    nameToClass.insert({"Brigand"          , Brigand         });
    nameToClass.insert({"Pirate"           , Pirate          });
    nameToClass.insert({"Corsair"          , Corsair         });
    nameToClass.insert({"Berserker"        , Berserker       });
    nameToClass.insert({"Archer"           , Archer          });
    nameToClass.insert({"Sniper"           , Sniper          });
    nameToClass.insert({"Nomad"            , Nomad           });
    nameToClass.insert({"Nomad Trooper"    , NomadTrooper    });
    nameToClass.insert({"Cavalier"         , Cavalier        });
    nameToClass.insert({"Paladin"          , Paladin         });
    nameToClass.insert({"Pegasus Knight"   , PegasusKnight   });
    nameToClass.insert({"Falcon Knight"    , FalconKnight    });
    nameToClass.insert({"Wyvern Rider"     , WyvernRider     });
    nameToClass.insert({"Wyvern Lord"      , WyvernLord      });
    nameToClass.insert({"Monk"             , Monk            });
    nameToClass.insert({"Cleric"           , Cleric          });
    nameToClass.insert({"Bishop"           , Bishop          });
    nameToClass.insert({"Troubadour"       , Troubadour      });
    nameToClass.insert({"Valkyrie"         , Valkyrie        });
    nameToClass.insert({"Mage"             , Mage            });
    nameToClass.insert({"Sage"             , Sage            });
    nameToClass.insert({"Archsage"         , Archsage        });
    nameToClass.insert({"Shaman"           , Shaman          });
    nameToClass.insert({"Druid"            , Druid           });
    nameToClass.insert({"Dark Druid"       , DarkDruid       });
    nameToClass.insert({"Bard"             , Bard            });
    nameToClass.insert({"Dancer"           , Dancer          });
    nameToClass.insert({"MagicSeal"        , MagicSeal       });
    nameToClass.insert({"Bramimond"        , Bramimond       });
    nameToClass.insert({"Fire Dragon"      , FireDragon      });
    nameToClass.insert({"Civilian"         , Civilian        });
    nameToClass.insert({"Prince"           , Prince          });
    nameToClass.insert({"Transporter Tent" , TransporterTent });
    nameToClass.insert({"Transporter Wagon", TransporterWagon});
}

std::string ClassData::getClassName(ClassType type)
{
    for (auto it = nameToClass.begin(); it != nameToClass.end(); ++it)
    {
        if (it->second == type)
        {
            return it->first;
        }
    } 

    return "Prince";
}

ClassType ClassData::getClassType(std::string name)
{
    auto a = nameToClass.find(name);
    if (a != nameToClass.end())
    {
        return a->second;
    }

    return Civilian;
}
