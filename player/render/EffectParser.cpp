/*
 *      Copyright (C) 2012 GuildTV
 *      http://www.guildtv.co.uk
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "EffectParser.h"

#include "OverlayRenderer.h"

bool EffectParser::Parse(OverlayRenderer* renderer, vector<TextureRender*> *renderElms, std::string file, Json::Value data) {
    filebuf fb;
    fb.open (file,ios::in);
    istream is(&fb);

    if(!fb.is_open()){
        FLog::Log(FLOG_ERROR, "EffectParser::Parse - Failed to load script file.");
        return false;
    }

    Json::Value root;
    Json::Reader reader;
    bool parsedSuccess = reader.parse(is, root, false);
    fb.close();
    if(!parsedSuccess){
        FLog::Log(FLOG_ERROR, "EffectParser::Parse - Failed to parse script json.");
        return false;
    }

     //check type exists
    if(!root.isMember("effects")){
        FLog::Log(FLOG_ERROR, "EffectParser::Parse - No list of effects");
        return false;
    }

    Json::Value e = root["effects"];

    for(Json::ValueIterator it = e.begin(); it != e.end(); it++){
        TextureRender *ren = ParseEffect(renderer, *it, &data);
        if(ren != NULL)
            renderElms->push_back(ren);
    }

    return true;
}

TextureRender* EffectParser::ParseEffect(OverlayRenderer* renderer, Json::Value effect, Json::Value *data) {
    TextureRender* parent;
    bool hasParent = false;

    if(!effect["parent"].empty()) {
        parent = ParseEffect(renderer, effect["parent"], data);
        hasParent = true;
    }

    if(hasParent && !parent){
        return NULL;
    }

    if(!effect.isMember("type") || !effect.isMember("data")){
        return parent;
    }

    std::string type = effect["type"].asString();

    if(!hasParent){
        //must be source effect
        if(type.compare("SolidTexture") == 0){
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect (SolidTexture) - created");
            return new SolidTexture(renderer);
        } else if(type.compare("TextTexture") == 0){
            TextTexture *t = new TextTexture(renderer);
            Json::Value dat = effect["data"];
            if(dat.isMember("name") && dat.isMember("posX") && dat.isMember("posY")){
                int posX = dat["posX"].asInt();
                int posY = dat["posY"].asInt();
                string name = dat["name"].asString();
                string value = name;
                if(data->isMember(name)){
                    value = data->get(name, dat["name"]).asString();
                }

                printf("hi - %s\n",value.c_str());

                t->setText(value.c_str(), renderer->getDefaultCharSet(), posX, posY, 1.0f, 1.0f);//TODO dynamicify the rest of this
            }
            return t;

        } else if(type.compare("PNGTexture") == 0){
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect (PNGTexture) - created");
            PNGTexture *t = new PNGTexture(renderer);
            Json::Value dat = effect["data"];
            if(dat.isMember("file")){
                if(JsonUtil::FileExists(dat["file"].asString().c_str()))
                    t->LoadPNG(dat["file"].asString());
                else
                    FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (PNGTexture) - file doesnt exist");
            } else
                FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (PNGTexture) - file not defined");

            return t;

        } else {
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect - unknown source");
            return NULL;
        }
    } else {
        //must be an actual effect
        if(type.compare("ColourTexture") == 0){
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect (ColourTexture) - create");
            ColourTexture *t = new ColourTexture(renderer, parent);
            Json::Value dat = effect["data"];
            if(dat.isMember("red") && dat.isMember("green") && dat.isMember("blue") && dat.isMember("alpha")){
                float red = dat["red"].asFloat();
                float green = dat["green"].asFloat();
                float blue = dat["blue"].asFloat();
                float alpha = dat["alpha"].asFloat();
                t->SetColour(red, green, blue, alpha);
            } else
                FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (ColourTexture) - missing colour data");

            return t;

        } else if(type.compare("FadeTexture") == 0){
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect (FadeTexture) - create");
            FadeTexture *t = new FadeTexture(renderer, parent);
            Json::Value dat = effect["data"];
            if(dat.isMember("points")){
                Json::Value points = dat["points"];
                if(points.isArray() && points.size() >= 1){
                    for(Json::ValueIterator it = points.begin(); it != points.end(); it++){
                        Json::Value p = *it;
                        if(p.isMember("field") && p.isMember("alpha")){
                            int field = p["field"].asInt();
                            float alpha = p["alpha"].asFloat();
                            t->addPoint(new FadePoint(field, alpha));
                        } else
                            FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (FadeTexture) - data point is invalid");
                    }
                } else
                    FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (FadeTexture) - no data array");
            } else
                FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (FadeTexture) - no data points");

            return t;

        } else if(type.compare("MaskTexture") == 0){
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect (MaskTexture) - create");
            MaskTexture *t = new MaskTexture(renderer, parent);
            Json::Value dat = effect["data"];
            if(dat.isMember("points")){
                Json::Value points = dat["points"];
                if(points.isArray() && points.size() >= 1){
                    for(Json::ValueIterator it = points.begin(); it != points.end(); it++){
                        Json::Value p = *it;
                        if(p.isMember("bl") && p.isMember("br") && p.isMember("tl") && p.isMember("tr") && p.isMember("field")){
                            Json::Value bl = p["bl"];
                            Json::Value br = p["br"];
                            Json::Value tl = p["tl"];
                            Json::Value tr = p["tr"];
                            if(bl.isMember("x") && bl.isMember("y") && br.isMember("x") && br.isMember("y") && tl.isMember("x") && tl.isMember("y") && tr.isMember("x") && tr.isMember("y")){
                                float blX = bl["x"].asFloat();
                                float blY = bl["y"].asFloat();
                                float brX = br["x"].asFloat();
                                float brY = br["y"].asFloat();
                                float tlX = tl["x"].asFloat();
                                float tlY = tl["y"].asFloat();
                                float trX = tr["x"].asFloat();
                                float trY = tr["y"].asFloat();
                                int field = p["field"].asInt();

                                t->addPoint(new MaskPoint(field, tlX, tlY, trX, trY, blX, blY, brX, brY));
                            } else
                                FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (MaskTexture) - data point is invalid");
                        } else
                            FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (MaskTexture) - data point is invalid");
                    }
                }
            } else
                FLog::Log(FLOG_ERROR, "EffectParser::ParseEffect (MaskTexture) - no data points");

            return t;

        } else {
            FLog::Log(FLOG_DEBUG, "EffectParser::ParseEffect - unknown effect");
            return NULL;
        }
    }

}
