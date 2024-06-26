
#ifndef net_MergeVec_h
#define net_MergeVec_h

#include <vector>

namespace net {

static int64_t makeLong64(int begin, int end)
{
    return (int64_t)((int64_t)begin << 32) + end;
}

static bool isInLine(int pos, int begin, int end)
{
    return ((pos >= begin - 0) && (pos <= end + 0));
}

struct PosInfo {
    PosInfo(bool mid, int posi)
    {
        this->isMiddle = mid;
        this->pos = posi;
    }
    bool ieEq(const PosInfo& other) const
    {
        return (isMiddle == other.isMiddle && pos == other.pos);
    }

    bool isMiddle; // 在线段的左边、还是中间
    int pos;  // 在哪一个线段
};
PosInfo scanLine(std::vector<int64_t>* ranges, int pos)
{
    for (size_t i = 0; i < ranges->size(); ++i) {
        int64_t it = ranges->at(i);
        int beginIt = it >> 32;
        int endIt = it & 0xffffffff;

        if (pos < beginIt)
            return PosInfo(false, i);
        else if (pos >= beginIt && pos <= endIt)
            return PosInfo(true, i);
        else
            continue;
    }
    return PosInfo(false, ranges->size());
}

static void validVec(const std::vector<int64_t>& ranges)
{
    for (size_t i = 0; i < ranges.size() - 1; ++i) {
        int begin = ranges.at(i) >> 32;
        int end = ranges.at(i) & 0xffffffff;
        if (!(begin <= end))
            DebugBreak();

        int beginNext = ranges.at(i + 1) >> 32;
        int endNext = ranges.at(i + 1) & 0xffffffff;
        if (!(begin <= end) || !(end < beginNext - 1))
            DebugBreak();
    }
}

static void removeNext(std::vector<int64_t>* ranges, int pos)
{
    int begin = ranges->at(pos) >> 32;
    int end = ranges->at(pos) & 0xffffffff;
    // -- --
    //  |
    int beginNext = ranges->at(pos + 1) >> 32;
    int endNext = ranges->at(pos + 1) & 0xffffffff;
    if (end >= beginNext - 1) {
        if (end > endNext)
            DebugBreak();
        ranges->at(pos) = makeLong64(begin, endNext);
        ranges->erase(ranges->begin() + pos + 1);
    }
}

static void removeLast(std::vector<int64_t>* ranges, int pos)
{
    int begin = ranges->at(pos) >> 32;
    int end = ranges->at(pos) & 0xffffffff;
    // -- --
    //    |
    int beginLast = ranges->at(pos - 1) >> 32;
    int endLast = ranges->at(pos - 1) & 0xffffffff;
    if (begin <= endLast + 1) {
        if (begin < beginLast)
            DebugBreak();
        ranges->at(pos - 1) = makeLong64(beginLast, end);
        ranges->erase(ranges->begin() + pos);
    }
}

void mergeTouchVec(std::vector<int64_t>* ranges, int pos)
{
    if (ranges->size() <= 1 || pos >= ranges->size())
        return;

    if (pos == 0) {
        removeNext(ranges, pos);
        return;
    } else if (pos == ranges->size() - 1) {
        removeLast(ranges, pos);
        return;
    } else {
        removeNext(ranges, pos);
        removeLast(ranges, pos);
    }
    validVec(*ranges);
}

void mergeVec(std::vector<int64_t>* ranges, int begin, int end)
{
    if (ranges->size() == 0) {
        ranges->push_back(makeLong64(begin, end));
        return;
    }

    PosInfo aInfo = scanLine(ranges, begin);
    PosInfo bInfo = scanLine(ranges, end);

    if (aInfo.ieEq(bInfo)) {
        if (aInfo.isMiddle) // 如果两个都是被包含状态，就不用管了
            return;
        ranges->insert(ranges->begin() + aInfo.pos, makeLong64(begin, end));
        mergeTouchVec(ranges, aInfo.pos);
        return;
    }

    int removeNum = 0;
    if (aInfo.isMiddle && bInfo.isMiddle) {
        //--   ----- ------
        //---------------
        removeNum = bInfo.pos - aInfo.pos;
    } else if (!aInfo.isMiddle && bInfo.isMiddle) {
        //    --   ---
        //-----------
        removeNum = bInfo.pos - aInfo.pos;
    } else if (aInfo.isMiddle && !bInfo.isMiddle) {
        //    --  --   ---
        //     ------
        removeNum = bInfo.pos - aInfo.pos - 1;
    } else if (!aInfo.isMiddle && !bInfo.isMiddle) {
        //    -- --     ---
        //  -----------
        removeNum = bInfo.pos - aInfo.pos - 1;
    }

    int begin2 = ranges->at(aInfo.pos) >> 32;
    begin2 = begin2 < begin ? begin2 : begin;

    int end2 = 0;
    if (bInfo.isMiddle)
        end2 = ranges->at(bInfo.pos) & 0xffffffff;
    else
        end2 = end;
    ranges->at(aInfo.pos) = makeLong64(begin2, end2);

    for (size_t i = aInfo.pos + 1, count = 0; count < removeNum; ++count) {
        ranges->erase(ranges->begin() + (i));
    }
    mergeTouchVec(ranges, aInfo.pos);
}

}

#endif // net_MergeVec_h