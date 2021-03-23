//
// Created by caolong on 2020/8/5.
//

#pragma once

#include <utils/Log.hpp>
#include <yaml-cpp/yaml.h>
#include <softarch/std.hpp>

namespace clt {

    class YamlParse {
    public:
        explicit YamlParse(const std::string &file) {
            std::fstream f;
            f.open(file, std::ios::in);
            if (!f.is_open()) {
                Log::w(Log::RES_TAG, "YamlParse %s invalid", file.c_str());
                m_valid = false;
            } else {
                m_valid = true;
                m_node = YAML::LoadFile(file);
            }
            f.close();
        }

        ~YamlParse() = default;

        template<typename T>
        T Parse(const std::string &name) const {
            return m_node[name].as<T>();
        }

        const YAML::Node &GetNode() const {
            return m_node;
        }

        bool HasToken(const std::string &name) const {
            return m_node[name].IsDefined();
        }

        bool Valid() const {
            return m_valid;
        }
    private:
        YAML::Node m_node;
        bool m_valid;
    };

    class YamlCreator {
    public:
        explicit YamlCreator(const std::string &file) {
            if (file.empty()) {
                Log::w(Log::RES_TAG, "YamlCreator %s invalid", file.c_str());
            }
            m_file = file;
        }

        YAML::Emitter &Emit() {
            return m_emitter;
        }

        void Save() {
            std::ofstream out(m_file);
            if (out.is_open()) {
                out << m_emitter.c_str();
                out.flush();
            } else {
                Log::w(Log::RES_TAG, "YamlCreator %s can't open file", m_file.c_str());
            }
            out.close();
        }

    private:
        std::string m_file;
        YAML::Emitter m_emitter;
    };
}

//    creator.Emit() << YAML::BeginMap;
//    creator.Emit() << YAML::Key << "camera_matrix";
//    creator.Emit() << YAML::Value;
//    creator.Emit() << YAML::BeginSeq;
//    creator.Emit() << YAML::Flow << matrix;
//    creator.Emit() << YAML::EndSeq;
//    creator.Emit() << YAML::EndMap;
//    //
//    creator.Emit() << YAML::BeginMap;
//    creator.Emit() << YAML::Key << "dist_coeffs";
//    creator.Emit() << YAML::Value;
//    creator.Emit() << YAML::BeginSeq;
//    creator.Emit() << YAML::Flow << dist;
//    creator.Emit() << YAML::EndSeq;
//    creator.Emit() << YAML::EndMap;
