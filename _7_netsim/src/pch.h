#pragma execution_character_set("utf-8")
#pragma once
#ifndef XX_EXAMPLES_7_H_
#define XX_EXAMPLES_7_H_

#include <xx2d.h>
#include <xx_space_ex.h>
using XY = xx::XY;
inline static constexpr float gPI = xx::gPI;
inline static constexpr float gNPI = xx::gNPI;
inline static constexpr float g2PI = xx::g2PI;
inline static constexpr float gPI_2 = xx::gPI_2;
#include <Fixed64.h>
using FX64 = Fixed64::FP_LONG;

namespace xx {

    struct SerdeBase {
        static constexpr uint16_t cTypeId{};
        static constexpr uint16_t cParentTypeId{};
        SerdeBase() = default;
        virtual ~SerdeBase() = default;
        virtual void Write(Data& d) const {};
        virtual int Read(Data_r& dr) { return __LINE__; }
        uint16_t typeId{};  // fill by maker
    };

    struct SerdeInfo {
        typedef Shared<SerdeBase> (*Func) ();
        inline static Shared<SerdeBase> null;

        std::array<Func, 65536> fs{};
        std::array<uint16_t, 65536> pids{};

		XX_INLINE bool IsBaseOf(uint16_t baseTypeId, uint16_t typeId) noexcept {
			for (; typeId != baseTypeId; typeId = pids[typeId]) {
				if (!typeId || typeId == pids[typeId]) return false;
			}
			return true;
		}

		template<typename BT>
		XX_INLINE bool IsBaseOf(uint16_t typeId) noexcept {
			static_assert(std::is_base_of_v<SerdeBase, BT>);
			return IsBaseOf(BT::cTypeId, typeId);
		}

		template<typename BT, typename T>
		XX_INLINE bool IsBaseOf() noexcept {
			static_assert(std::is_base_of_v<SerdeBase, T>);
			static_assert(std::is_base_of_v<SerdeBase, BT>);
			return IsBaseOf(BT::cTypeId, T::cTypeId);
		}

		template<typename T>
		XX_INLINE void Register() {
			static_assert(std::is_base_of_v<SerdeBase, T>);
			pids[T::cTypeId] = T::cParentTypeId;
			fs[T::cTypeId] = []() -> Shared<SerdeBase> { return MakeShared<T, true>(); };
		}

        template<typename T = SerdeBase>
        XX_INLINE Shared<T> Create(uint16_t const& typeId) {
            static_assert(std::is_base_of_v<SerdeBase, T>);
            if (!typeId || !fs[typeId] || !IsBaseOf<T>(typeId)) return nullptr;
			return (Shared<T>&&)fs[typeId]();
		}
    };

    struct DataEx_r : Data_r {
        using Data_r::Data_r;
        SerdeInfo* si{};                            // need fill before use
        xx::Listi32<xx::Shared<SerdeBase>> ptrs;    // need clear after read finish
    };

    struct UdPtr {
        size_t* p;
        UdPtr(size_t* p) : p(p) {}
        UdPtr(UdPtr const&) = default;
        UdPtr& operator=(UdPtr const&) = default;
        ~UdPtr() {
            *p = 0;
        }
    };

    struct DataEx : Data {
        using Data::Data;
        SerdeInfo* si{};                            // need fill before use
        xx::Listi32<UdPtr> ptrs;                    // need clear after write finish
    };

    template<typename T>
    struct DataFuncs<T, std::enable_if_t<xx::IsShared_v<T> && std::is_base_of_v<SerdeBase, typename T::ElementType>>> {
        using U = typename T::ElementType;
        //static_assert(TypeId_v<U> > 0);

        template<bool needReserve = true>
        static inline void Write(Data& d, T const& in) {
            if (!in) {
                d.WriteFixed<needReserve>((uint8_t)0);              // index( nullptr ) same as WriteVar(size_t 0)
            } else {
                auto& ptrs = ((DataEx&)d).ptrs;
                auto h = in.GetHeader();
                if (h->ud == 0) {                   // first time
                    ptrs.Emplace(&h->ud);           // for restore
                    h->ud = (size_t)ptrs.len;       // store index
                    d.WriteVarInteger<needReserve>(h->ud);          // index
                    d.WriteVarInteger<needReserve>(in->typeId);     // type id
                    in->WriteTo(d);                                 // content
                } else {                            // exists
                    d.WriteVarInteger<needReserve>(h->ud);          // index
                }
            }
        }

        static inline int Read(Data_r& dr, T& out) {
            size_t idx;
            if (int r = dr.Read(idx)) return r;                 // index
            if (!idx) {                             // nullptr
                out.Reset();
                return 0;
            }
            --idx;
            auto& ptrs = ((DataEx_r&)dr).ptrs;
            auto& si = *((DataEx_r&)dr).si;
            auto len = (size_t)ptrs.len;
            if (idx == len) {                       // first time
                uint16_t typeId;
                if (int r = dr.Read(typeId)) return r;          // type id
                if (!typeId) return __LINE__;
                if (!si.fs[typeId]) return __LINE__;
                if (!si.IsBaseOf<U>(typeId)) return __LINE__;

                if (!out || out->typeId != typeId) {
                    out = std::move(si.Create<U>(typeId));
                    assert(out);
                }
                ptrs.Emplace(out);
                return out->ReadFrom(dr);
            } else {
                if (idx > len) return __LINE__;
                auto& o = ptrs[idx];
                if (!si.IsBaseOf<U>(o->typeId)) return __LINE__;
                out = (Shared<U>&)o;
                return 0;
            }
        }
    };
    
    template<typename T>
    struct DataFuncs<T, std::enable_if_t<xx::IsWeak_v<T> && std::is_base_of_v<SerdeBase, typename T::ElementType>>> {
        using U = typename T::ElementType;
        //static_assert(TypeId_v<U> > 0);

        template<bool needReserve = true>
        static inline void Write(Data& d, T const& in) {
            if (!in) {
                d.WriteFixed<needReserve>((uint8_t)0);              // index( nullptr ) same as WriteVar(size_t 0)
            } else {
                auto p = &in.h->data;
                d.Write<needReserve>((Shared<U>&)p);
            }
        }

        static inline int Read(Data_r& dr, T& out) {
            Shared<U> o;
            if (int r = dr.Read(o)) return r;
            out = o;
            return 0;
        }
    };

}

#endif
