#pragma once

#include <cctype>
#include <charconv>
#include <algorithm>

#include "SceneType.hpp"
#include "TagActorType.hpp"
#include "ActorType.hpp"
#include "TagsConstants.hpp"
#include "ActorTypeDefs.hpp"
#include "ActorPosition.hpp"
#include "Authors.hpp"
#include "Contact.hpp"
#include "Furniture.hpp"
#include "Narrative.hpp"
#include "SceneAttributes.hpp"
#include "SceneServiceAttributes.hpp"

#include "ActorTypeNames.hpp"
#include "common/IDebugLog.h"

namespace Proc
{
    using namespace Parsing;
    using namespace AAFXML;

    inline std::string ToUpper(const std::string s)
    {
        // All tags are in ASCII, so we don't care about locale during case conversion
        std::string copy = s;
        std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::toupper(c); });
        return copy;
    }


    struct SceneDetails
    {

        SceneDetails()
	        :Type(SceneType::Neutral)
            ,HeldLevel(0)
            ,StimLevel(0)
            ,LoveLevel(0)
			,DomLevel(0)
        {

        };

        SceneType Type;
        // numeric attributes
        int HeldLevel; // [0, 9] , 0 - free, 9 - max restrained
        int StimLevel; // [-9, 9] , negative values - pain
        int LoveLevel; // [-9, 9] , negative values - hate
        int DomLevel; // [-9, 9] , defined for 0 actor only.negative values - submissiveness

        std::string Author; //if known
        std::string Narrative;
        std::string Attributes;
        std::string Service;
        std::string Furniture; //if recognized
        std::vector<ActorType> Actors;
        std::vector<Proc::TagActorType> TagActors;
        
        std::vector<Proc::ActorPosition> Positions;
        // index of outer vector is same to Actor's index
        // inner vector is list of From/To pairs where To is applied to actor with index of outer vector
        std::vector<std::vector<From>> FromContacts;
        std::vector<std::vector<To>> ToContacts;
        std::string Unparsed; //tags that are not recognized. separated by comma
        std::vector<std::string> Warnings; // some meaningful messages about anomalies in input tags
    };

    struct TagsCategorizer
    {
        // if line hasn't any underscore symbol then it is not a tag that represent actors
        static std::vector<TagActorType> ParseActors(const std::string_view actorsLine)
        {
            std::vector<TagActorType> retVal;
            size_t startIndex = 0;
            size_t usIndex = actorsLine.find_first_of('_', startIndex);
            while (usIndex != std::string::npos)
            {
                const std::string_view lexem(actorsLine.data() + startIndex, usIndex - startIndex);

                const auto ptr = actorTypesMap.find(lexem);
                if (ptr == actorTypesMap.end())
                {
                    break;
                }

                retVal.push_back(ptr->second);

                startIndex = usIndex + 1;
                if (startIndex >= actorsLine.size())
                    break;
                usIndex = actorsLine.find_first_of('_', startIndex);
            }
            if (startIndex < actorsLine.size())
            {
                const std::string_view lexem(actorsLine.data() + startIndex);
                const auto ptr = actorTypesMap.find(lexem);
                if (ptr == actorTypesMap.end())
                {
                    return std::vector<TagActorType>();
                }

                retVal.push_back(ptr->second);
            }
            return retVal;
        }

        static SceneDetails GetSceneDetails(const std::vector<ActorType>& actors, const std::vector<std::string>& tags)
        {
            SceneDetails retVal;
            retVal.Positions.assign(actors.size(), ActorPosition::NoPosition);
            retVal.FromContacts.assign(actors.size(), std::vector<From>());
            retVal.ToContacts.assign(actors.size(), std::vector<To>());

            bool hasAuthor = false;
            size_t contactIndex = 0;
            for (const std::string& t : tags)
            {
                auto upper = ToUpper(t);
                const auto tag = std::string_view(upper);
                if (!hasAuthor && TryFillAuthor(retVal, tag, t))
                {
                    hasAuthor = true;
                    continue;
                }

            	if (TryFillSimples(retVal, tag))
					continue;

                if (!TryGetAutor(tag).empty())
                { // ignore parsed actor type
                    continue;
                }

                if (TryFillNumericValues(retVal, tag))
                    continue;

                if (TryFillContacts(retVal, tag, contactIndex))
                    continue;

                std::vector<TagActorType> tagActors = ParseActors(tag);
                if (!tagActors.empty())
                { // ignore parsed actor type
                    continue;
                }

                if (!retVal.Unparsed.empty())
                    retVal.Unparsed += ",";
                retVal.Unparsed += t;
            }
            retVal.Actors = actors;
            return retVal;
        }

        static bool TryFillSimples(SceneDetails& retVal, const std::string_view tag)
        {
            if (TryFillFurniture(retVal, tag))
                return true;

            if (TryFillNarrative(retVal, tag))
                return true;

            if (TryFillAttributes(retVal, tag))
                return true;

            if (TryFillService(retVal, tag))
                return true;

            if (TryFillPosition(retVal, tag))
                return true;

            return TryFillType(retVal, tag);
        }

        static bool TryFillAuthor(SceneDetails& retVal, const std::string_view tag, const std::string& t)
        {
            if (authors.find(tag) != authors.end())
            {
                retVal.Author = t;
                return true;
            }
            return false;
        }
        static bool TryFillFurniture(SceneDetails& retVal, const std::string_view tag)
        {
            if (furniture.find(tag) != furniture.end())
            {
                if (!retVal.Furniture.empty())
                    retVal.Furniture += ",";
                retVal.Furniture += tag;
                return true;
            }
            return false;
        }
        static bool TryFillNarrative(SceneDetails& retVal, const std::string_view tag)
        {
            if (sceneNarrative.find(tag) != sceneNarrative.end())
            {
                if (!retVal.Narrative.empty())
                    retVal.Narrative += ",";
                retVal.Narrative += tag;
                return true;
            }
            return false;
        }
        static bool TryFillAttributes(SceneDetails& retVal, const std::string_view tag)
        {
            if (sceneAttributes.find(tag) != sceneAttributes.end())
            {
                if (!retVal.Attributes.empty())
                    retVal.Attributes += ",";
                retVal.Attributes += tag;
                return true;
            }
            return false;
        }
        static bool TryFillService(SceneDetails& retVal, const std::string_view tag)
        {
            if (sceneServiceAttributes.find(tag) != sceneServiceAttributes.end())
            {
                if (!retVal.Service.empty())
                    retVal.Service += ",";
                retVal.Service += tag;
                return true;
            }
            return false;
        }
        static bool TryFillPosition(SceneDetails& retVal, const std::string_view tag)
        {
            auto apPtr = actorsPosition.find(tag);
            if (apPtr != actorsPosition.end())
            {
                retVal.Positions[0] |= apPtr->second;
                return true;
            }
            return false;
        }
        static bool TryFillType(SceneDetails& retVal, const std::string_view tag)
        {
            auto iter = sceneTypesMap.find(tag);
            if (iter != sceneTypesMap.end())
            {
                retVal.Type += iter->second;
                return true;
            }
            return false;
        }
        static std::vector<TagActorType> TryGetAutor(const std::string_view tag)
        {
            auto ptr = singleActorDefs.find(tag);
            if (ptr != singleActorDefs.end())
            {
                return ptr->second;
            }
            return std::vector<TagActorType>();
        }
        static bool TryFillNumericValues(SceneDetails& retVal, const std::string_view tag)
        {
            int val;
            // parse Dom/Stim/etc. values
            if (sscanf_s(tag.data(), DomParseFormat, &val) == 1)
            {
                retVal.DomLevel = val;
                return true;
            }
            if (sscanf_s(tag.data(), HeldParseFormat, &val) == 1)
            {
                retVal.HeldLevel = val;
                return true;
            }
            if (sscanf_s(tag.data(), LoveParseFormat, &val) == 1)
            {
                retVal.LoveLevel = val;
                return true;
            }
            if (sscanf_s(tag.data(), StimParseFormat, &val) == 1)
            {
                retVal.StimLevel = val;
                return true;
            }
            return false;
        }
        static bool TryFillContacts(SceneDetails& retVal, const std::string_view tag, size_t& contactIndex)
        {
            const size_t delimIndex = tag.find("TO", 1); // 1 for handling Tonque
            if (delimIndex != std::string_view::npos)
            {
                if (tag == NullToSelf)
                {
                    // seems useless as it always applied to actor0
                    return true;
                }
                const std::string_view from(tag.data(), delimIndex + 2);
                const std::string_view to(tag.data() + delimIndex);
                // it is not possible to calculate what actors are contacted if their number greater than 2
                // so we assume that all contacts from actor1+ points to actor0
                // TODO: push to preparing tags XML that contains all necessary information

                const auto ptrFrom = simpleFrom.find(from);
                const auto ptrTo = simpleTo.find(to);
                if (ptrFrom != simpleFrom.end() && ptrTo != simpleTo.end())
                {
                    // To actor0
                    retVal.ToContacts[0].push_back(ptrTo->second);
                    contactIndex++;
                    if (contactIndex >= retVal.FromContacts.size())
                        contactIndex = retVal.FromContacts.size() - 1; // set all contacts to last participant
                    // From corresponding actor
                    retVal.FromContacts[contactIndex].push_back(ptrFrom->second);
                    _MESSAGE("  Tag=%s: [0]=%d, [%d]=%d", std::string(tag).c_str(), ptrTo->second, contactIndex, ptrFrom->second);

                    return true;
                }
            }
            return false;
        }

#ifdef PARSE_ACTOR_TAGS

        static std::string GetActorTypeName(TagActorType type)
        {
            auto ptr = actorTypeNames.find(type);
            if (ptr != actorTypeNames.end())
                return std::string(ptr->second);
            type = TagActorType(int(type) & ~3);
            ptr = actorTypeNames.find(type);
            if (ptr != actorTypeNames.end())
                return std::string(ptr->second);
            return "Missing";
        }

        // returns new vector
        static std::vector<TagActorType> MergeTypes(const std::vector<TagActorType>& a, const std::vector<TagActorType>& b, std::vector<std::string>& warnings)
        {
            warnings.emplace_back("Duplicate actors definition");
            std::vector<TagActorType> retVal;
            size_t max = std::max<size_t>(a.size(), b.size());
            for (size_t i = 0; i < max; i++)
            {
                TagActorType at(TagActorType::NoType);
                TagActorType bt(TagActorType::NoType);
                if (i < a.size())
                    at = a[i];
                if (i < b.size())
                    bt = b[i];

                // fix potentially invalid line
                int sum;
                if (at == bt)
                { // do not sum identical types
                    char message[100];
                    sprintf_s<100>(message, "Repeated same type %s", GetActorTypeName(at).c_str());
                    warnings.emplace_back(message);
                    sum = at;
                }
                else
                {
                    if (at <= TagActorType::F && bt <= TagActorType::F)
                        sum = at + bt;
                    else // do not merge non human actor types
                    {
                        // sum = std::max(at, bt); // probably wrong 

                        warnings.emplace_back("Incompatible actor types. Selected best guess");
                        // Do not continue to merge
                        // just return largest or first list
                        if (a.size() == b.size())
                            return a;
                        if (a.size() > b.size())
                            return a;
                        return b;
                    }
                }
                if (sum > TagActorType::P)
                    sum = TagActorType::P;

                retVal.push_back(static_cast<TagActorType>(sum));
            }

            return retVal;
        }


        static SceneDetails GetSceneFromTags(const std::vector<std::string>& tags)
        {
            SceneDetails retVal;
            retVal.Positions.assign(1, ActorPosition::NoPosition);
            bool hasAuthor = false;
            bool actorsFound = false;
            for (auto& t : tags)
            {
                auto upper = ToUpper(t);
                const auto tag = std::string_view(upper);
                if (!hasAuthor && TryFillAuthor(retVal, tag, t))
                {
                    hasAuthor = true;
                    continue;
                }

                if (TryFillSimples(retVal, tag))
                    continue;

                std::vector<TagActorType> simpleActors = TryGetAutor(tag);
                if (!simpleActors.empty())
                {
                    retVal.TagActors = simpleActors;
                    actorsFound = true;
                    continue;
                }
                if (TryFillNumericValues(retVal, tag))
                    continue;


                const std::vector<TagActorType> tagActors = ParseActors(tag);
                if (!tagActors.empty())
                {
                    if (actorsFound)
                        retVal.TagActors = MergeTypes(retVal.TagActors, tagActors, retVal.Warnings);
                    else
                        retVal.TagActors = tagActors;
                    actorsFound = true;
                    continue;
                }

                const size_t delimIndex = tag.find("TO", 1); // 1 for handling Tonque
                if (delimIndex != std::string_view::npos)
                {
                    if (tag == NullToSelf)
                        continue;
                    const std::string_view from(tag.data(), delimIndex + 2);
                    const std::string_view to(tag.data() + delimIndex);
                    const auto ptrFrom = simpleFrom.find(from);
                    const auto ptrTo = simpleTo.find(to);
                    if (ptrFrom != simpleFrom.end() && ptrTo != simpleTo.end())
                        continue;
                }

                if (!retVal.Unparsed.empty())
                    retVal.Unparsed += ",";
                retVal.Unparsed += t;
            }
            return retVal;
        }
#endif
    };
};