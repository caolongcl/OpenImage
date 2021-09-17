//
// Created by caolong on 2020/7/22.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {
/**************************************************************************************************/
/**
 * 用于实际存储在对象中的类型
 */
    struct Float1 {
        Float1() : x(0.0f) {}

        Float1(const float _x) : x(_x) {}

        union {
            float x;
            float r;
            float s;
        };
    };

    struct Float2 {
        Float2() : x(0.0f), y(0.0f) {}

        Float2(const float _x, const float _y)
                : x(_x), y(_y) {}

        union {
            struct {
                float x, y;
            };
            struct {
                float r, g;
            };
            struct {
                float s, t;
            };
            struct {
                float w, h;
            };
        };
    };

    struct Float3 {
        Float3() : x(0.0f), y(0.0f), z(0.0f) {}

        Float3(const float _x, const float _y, const float _z)
                : x(_x), y(_y), z(_z) {}

        union {
            struct {
                float x, y, z;
            };
            struct {
                float r, g, b;
            };
            struct {
                float s, t, p;
            };
        };
    };

    struct Float4 {
        Float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

        Float4(const float _x, const float _y, const float _z, const float _w)
                : x(_x), y(_y), z(_z), w(_w) {}

        union {
            struct {
                float x, y, z, w;
            };
            struct {
                float r, g, b, a;
            };
            struct {
                float s, t, p, q;
            };
        };
    };

    struct Integer1 {
        Integer1() : x(0) {}

        Integer1(const int _x) : x(_x) {}

        union {
            int x;
            int r;
            int s;
        };
    };

    struct Integer2 {
        Integer2() : x(0), y(0) {}

        Integer2(const int _x, const int _y)
                : x(_x), y(_y) {}

        union {
            struct {
                int x, y;
            };
            struct {
                int r, g;
            };
            struct {
                int s, t;
            };
            struct {
                int w, h;
            };
        };
    };

    struct Integer3 {
        Integer3() : x(0), y(0), z(0) {}

        Integer3(const int _x, const int _y, const int _z)
                : x(_x), y(_y), z(_z) {}

        union {
            struct {
                int x, y, z;
            };
            struct {
                int r, g, b;
            };
            struct {
                int s, t, p;
            };
        };
    };

    struct Integer4 {
        Integer4() : x(0), y(0), z(0), w(0) {}

        Integer4(const int _x, const int _y, const int _z, const int _w)
                : x(_x), y(_y), z(_z), w(_w) {}

        union {
            struct {
                int x, y, z, w;
            };
            struct {
                int r, g, b, a;
            };
            struct {
                int s, t, p, q;
            };
        };
    };

    using Color = Float4;
    static Color RedColor{1.0f, 0.0f, 0.0f, 1.0f};
    static Color GreenColor{0.0f, 1.0f, 0.0f, 1.0f};
    static Color BlueColor{0.0f, 0.0f, 1.0f, 1.0f};
    static Color WhiteColor{1.0f, 1.0f, 1.0f, 1.0f};
    static Color BlackColor{0.0f, 0.0f, 0.0f, 1.0f};
    static Color TransColor{0.0f, 0.0f, 0.0f, 0.0f};
/**************************************************************************************************/
    /**
     * 用于传递参数，标识参数的类型
     */
    struct TypeTag {
        enum {
            FLOAT_START = 1, INT_START = 5, BOOL_START = 9, STRING_START = 10
        };

        TypeTag() : id(0) {}

        TypeTag(int _id) : id(_id) {}

        friend bool operator==(const TypeTag &type1, const TypeTag &type2) {
            return type1.id == type2.id;
        }

        bool IsFloat() const { return id >= 1 && id < 5; }

        bool IsInteger() const { return id >= 5 && id < 9; }

        bool IsBoolean() const { return id == 9; }

        bool IsString() const { return id == 10; }

        int id{0};
    };

    struct Float1TypeTag : public TypeTag {
        Float1TypeTag() : TypeTag(TypeTag::FLOAT_START) {}
    };

    struct Float2TypeTag : public TypeTag {
        Float2TypeTag() : TypeTag(TypeTag::FLOAT_START + 1) {}
    };

    struct Float3TypeTag : public TypeTag {
        Float3TypeTag() : TypeTag(TypeTag::FLOAT_START + 2) {}
    };

    struct Float4TypeTag : public TypeTag {
        Float4TypeTag() : TypeTag(TypeTag::FLOAT_START + 3) {}
    };

    struct Int1TypeTag : public TypeTag {
        Int1TypeTag() : TypeTag(TypeTag::INT_START) {}
    };

    struct Int2TypeTag : public TypeTag {
        Int2TypeTag() : TypeTag(TypeTag::INT_START + 1) {}
    };

    struct Int3TypeTag : public TypeTag {
        Int3TypeTag() : TypeTag(TypeTag::INT_START + 2) {}
    };

    struct Int4TypeTag : public TypeTag {
        Int4TypeTag() : TypeTag(TypeTag::INT_START + 3) {}
    };

    struct BoolTypeTag : public TypeTag {
        BoolTypeTag() : TypeTag(TypeTag::BOOL_START) {}
    };

    struct StringTypeTag : public TypeTag {
        StringTypeTag() : TypeTag(TypeTag::STRING_START) {}
    };

    /**
     * 数值类型
     * @tparam T
     * @tparam ID_BASE
     */
    template<typename T, int ID_BASE>
    struct Numerical {
        Numerical() : ta{0} {}

        Numerical(T _x) : x(_x), tag(ID_BASE) {}

        Numerical(T _x, T _y) : x(_x), y(_y), tag(ID_BASE + 1) {}

        Numerical(T _x, T _y, T _z) : x(_x), y(_y), z(_z), tag(ID_BASE + 2) {}

        Numerical(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w), tag(ID_BASE + 3) {}

        Numerical(const T *elems, int count)
                : tag(ID_BASE + count - 1) {
            assert(count > 0 && count <= 4);
            for (int i = 0; i < count; ++i) {
                ta[i] = elems[i];
            }
        }

        union {
            struct {
                T x, y, z, w;
            };
            struct {
                T r, g, b, a;
            };
            struct {
                T s, t, p, q;
            };
            T ta[4];
        };

        T &operator[](int i) {
            assert(i >= 0 && i < 4);
            return ta[i];
        }

        T const &operator[](int i) const {
            assert(i >= 0 && i < 4);
            return ta[i];
        }

        TypeTag tag;

        const std::string Dump() const {
            std::stringstream ss;
            std::string info;

            switch (tag.id) {
                case TypeTag::FLOAT_START:
                    ss << "float{" << ta[0] << "}";
                    ss >> info;
                    break;
                case TypeTag::FLOAT_START + 1:
                    ss << "float2{" << ta[0] << " " << ta[1] << "}";
                    ss >> info;
                    break;
                case TypeTag::FLOAT_START + 2:
                    ss << "float3{" << ta[0] << " " << ta[1] << " " << ta[2] << "}";
                    ss >> info;
                    break;
                case TypeTag::FLOAT_START + 3:
                    ss << "float4{" << ta[0] << " " << ta[1] << " " << ta[2] << " " << ta[3] << "}";
                    ss >> info;
                    break;
                case TypeTag::INT_START:
                    ss << "int{" << ta[0] << "}";
                    ss >> info;
                    break;
                case TypeTag::INT_START + 1:
                    ss << "int2{" << ta[0] << " " << ta[1] << "}";
                    ss >> info;
                    break;
                case TypeTag::INT_START + 2:
                    ss << "int3{" << ta[0] << " " << ta[1] << " " << ta[2] << "}";
                    ss >> info;
                    break;
                case TypeTag::INT_START + 3:
                    ss << "int4{" << ta[0] << " " << ta[1] << " " << ta[2] << " " << ta[3] << "}";
                    ss >> info;
                    break;
                default:
                    info = "invalid type";
                    break;
            }
            return info;
        }
    };

    /**
     * 布尔类型
     */
    struct BooleanType {
        BooleanType() : b(false), tag(TypeTag::BOOL_START) {}

        BooleanType(bool _b) : b(_b), tag(TypeTag::BOOL_START) {}

        const std::string Dump() const { return "bool{" + std::string(b ? "true" : "false") + "}"; }

        bool b;
        TypeTag tag;
    };

    /**
     * 字符串类型
     */
    struct StringType {
        StringType() : str(), tag(TypeTag::STRING_START) {};

        StringType(const std::string &s) : str(s), tag(TypeTag::STRING_START) {};

        const std::string Dump() const { return "string{" + str + "}"; }

        std::string str;
        TypeTag tag;

    };

    using Float = Numerical<float, TypeTag::FLOAT_START>;
    using Integer = Numerical<int, TypeTag::INT_START>;
    using Boolean = BooleanType;
    using String = StringType;

/**************************************************************************************************/
/**
 * 参数封装，将浮点、整形、布尔、字符串统一起来
 */
    using VarType = TypeTag;

    struct Var {
        Var(const Float &_f) : type(_f.tag), fv(_f) {}

        Var(const Float1 &_f) : type(Float1TypeTag()), fv(_f.x, 0, 0, 0) {}

        Var(const Float2 &_f) : type(Float2TypeTag()), fv(_f.x, _f.y, 0, 0) {}

        Var(const Float3 &_f) : type(Float3TypeTag()), fv(_f.x, _f.y, _f.z, 0) {}

        Var(const Float4 &_f) : type(Float4TypeTag()), fv(_f.x, _f.y, _f.z, _f.w) {}

        Var(const Integer &_i) : type(_i.tag), iv(_i) {}

        Var(const Boolean &_b) : type(_b.tag), bv(_b) {}

        Var(const bool _b) : type(BoolTypeTag()), bv(_b) {}

        const Float &ToFloat() const {
            assert(type.IsFloat());
            return fv;
        }

        Float1 ToFloat1() const {
            assert(type.IsFloat());
            return Float1{fv.x};
        }

        Float2 ToFloat2() const {
            assert(type.IsFloat());
            return Float2{fv.x, fv.y};
        }

        Float3 ToFloat3() const {
            assert(type.IsFloat());
            return Float3{fv.x, fv.y, fv.z};
        }

        Float4 ToFloat4() const {
            assert(type.IsFloat());
            return Float4{fv.x, fv.y, fv.z, fv.w};
        }

        const Integer &ToInteger() const {
            assert(type.IsInteger());
            return iv;
        }

        const Boolean &ToBoolean() const {
            assert(type.IsBoolean());
            return bv;
        }

        const std::string Dump() const {
            if (type.IsFloat()) return fv.Dump();
            else if (type.IsInteger()) return iv.Dump();
            else if (type.IsBoolean()) return bv.Dump();
            return "Var{}";
        }

        static bool TypeIdentity(const Var &var, const VarType &type) {
            return var.type == type;
        }

    private:
        VarType type;
        union {
            Float fv;
            Integer iv;
            Boolean bv;
        };
    };

//    template<typename T, typename R = typename
//    std::enable_if<std::is_same<T, Float>::value ||
//                   std::is_same<T, Integer>::value ||
//                   std::is_same<T, Boolean>::value ||
//                   std::is_same<T, String>::value, typename T::var_type>::type>
//    static R ToVar(T &&s) {
//        return R(std::forward<T>(s));
//    }

/**************************************************************************************************/
    /**
     * 参数组数据设置接口
     * 主要用于着色器参数设置
     */
    struct IVarSet {
        virtual ~IVarSet() = default;

        virtual bool VarIn(const std::string &varName) = 0;

        virtual void VarSet(const std::string &varName, const Var &var) = 0;
    };

    /**
     * 参数设置器
     */
    struct Setter {
        using VarSetter = std::function<void(const Var &)>;

        Setter(const VarSetter &s, const VarType &t)
                : setter(s), type(t) {
        }

        bool operator()(const Var &var) {
            return Var::TypeIdentity(var, type) && (setter(var), true);
        }

        VarSetter setter;
        VarType type;
    };

    struct IVarRegister {
        virtual ~IVarRegister() = default;

        virtual void VarRegister(const std::string &varName, Setter &&setter) = 0;
    };

    struct VarGroup final : public IVarSet, public IVarRegister {
        /**
         * 检查是否支持此参数
         * @param varName
         * @return
         */
        bool VarIn(const std::string &varName) override {
            return vars.find(varName) != vars.cend();
        }

        /**
         * 注册支持的参数
         * @param varName
         * @param setter 参数设置器
         */
        void VarRegister(const std::string &varName, Setter &&setter) override {
            if (!VarIn(varName)) {
                vars.emplace(varName, std::forward<Setter>(setter));
            }
        }

        /**
         * 参数设置
         * @param varName
         * @param var
         */
        void VarSet(const std::string &varName, const Var &var) override {
            if (VarIn(varName)) {
                vars.find(varName)->second(var);
            }
        }

    private:
        std::unordered_map<std::string, Setter> vars;
    };

}