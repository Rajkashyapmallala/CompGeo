#include <bits/stdc++.h>
using namespace std;

//Node Attributes
#define UNERASED false
#define ERASED true
#define UNVISITED false
#define VISITED true
#define LEFT -1
#define UNMARKED 0
#define RIGHT 1
#define UNUSED false
#define USED true
int INFNITY = 0;

class Bloom
{

public:
    bool defined = false;
    pair<pair<int, int>, pair<int, int>> peaks;
    pair<int, int> base;
    bool isDefined()
    {
        return defined;
    }
    bool setDefined()
    {
        defined = true;
    }
};

pair<int, int> None = {INT_MIN, INT_MIN};
Bloom none;

class DfsInfo
{
public: //try to move this down
    pair<int, int> s;
    pair<int, int> t;
    pair<int, int> vL;
    pair<int, int> vR;
    pair<int, int> dcv;
    pair<int, int> barrier;

    DfsInfo(pair<int, int> s, pair<int, int> t, pair<int, int> vL, pair<int, int> vR, pair<int, int> dcv, pair<int, int> barrier)
    {
        s = s;
        t = t;
        vL = vL;
        vR = vR;
        dcv = dcv;
        barrier = barrier;
    }
};

class MicaliVazirani
{
    vector<pair<int, int>> nodes;
    map<pair<int, int>, vector<pair<int, int>>> neighbors; //all neightbors to a node
    map<pair<int, int>, map<pair<int, int>, map<string, bool>>> edges;
    MicaliVazirani(vector<pair<int, int>> nodes, map<pair<int, int>, vector<pair<int, int>>> neighbors, map<pair<int, int>, map<pair<int, int>, map<string, bool>>> edges)
    {
        nodes = nodes;
        neighbors = neighbors;
        edges = edges;
        INFNITY = nodes.size() + 1; //correct?
    }
    map<pair<int, int>, int> nodeEvenLevel;
    map<pair<int, int>, int> nodeOddLevel;
    map<pair<int, int>, Bloom> nodeBloom;
    map<pair<int, int>, vector<pair<int, int>>> nodePredecessors;
    map<pair<int, int>, vector<pair<int, int>>> nodeSuccessors;
    map<pair<int, int>, vector<pair<int, int>>> nodeAnomalies;
    map<pair<int, int>, int> nodeCount;
    map<pair<int, int>, bool> nodeErase;
    map<pair<int, int>, bool> nodeVisit;
    map<pair<int, int>, bool> nodeMark;
    map<pair<int, int>, pair<int, int>> nodeParent; //change def?

    vector<pair<int, int>> bloomNodes;
    map<int, vector<pair<int, int>>> candidates;
    map<int, set<pair<pair<int, int>, pair<int, int>>>> bridges; //change def?

    map<pair<int, int>, pair<int, int>> mate; //note all instances of != (none cases in python)

    map<pair<int, int>, pair<int, int>> max_cardinality_matching()
    {
        if (nodes.size() == 0)
            return {};

        bool augmented = true;
        while (augmented)
        {
            for (auto v : nodes)
            {
                nodeEvenLevel[v] = INFNITY;
                nodeOddLevel[v] = INFNITY;
                nodeBloom[v] = none;
                nodePredecessors[v].clear();
                nodeSuccessors[v].clear();
                nodeAnomalies[v].clear();
                nodeCount[v] = 0;
                nodeErase[v] = UNERASED;
                nodeVisit[v] = UNVISITED;
                nodeMark[v] = UNMARKED;
                nodeParent[v] = None;
            }

            for (auto e : edges)
            {
                for (auto v : e.second)
                {
                    edges[e.first][v.first]["use"] = UNUSED;
                    edges[e.first][v.first]["visit"] = UNVISITED;
                }
            }

            for (int i = 0; i < nodes.size() + 1; ++i)
            {
                candidates[i] = {};
                bridges[i] = {};
            }

            augmented = search();
        }

        //delete stuff

        return mate;
    }

    bool search()
    {
        int i = 0;
        for (auto v : nodes)
        {
            if (mate.find(v) == mate.end())
            {
                nodeEvenLevel[v] = 0;
                candidates[0].push_back(v);
            }
        }
        bool augmented = false;
        while (i < nodes.size() + 1 and !augmented)
        {
            if (i % 2 == 0)
            {
                for (auto v : candidates[i])
                {
                    for (auto u : neighbors[v])
                    {
                        if (u == v)
                            continue;
                        if (mate[v] != u and nodeErase[u] == UNERASED)
                        {
                            if (nodeEvenLevel[u] < INFNITY)
                            {
                                int j = (nodeEvenLevel[u] + nodeEvenLevel[v]) / 2;
                                bridges[j].insert({u, v}); //supposed to be sorted
                            }
                            else
                            {
                                if (nodeOddLevel[u] == INFNITY)
                                    nodeOddLevel[u] = i + 1;
                                if (nodeOddLevel[u] == i + 1)
                                {
                                    nodeCount[u] += 1;
                                    nodePredecessors[u].push_back(v);
                                    nodeSuccessors[v].push_back(u);
                                    candidates[i + 1].push_back(u);
                                }
                                else if (nodeOddLevel[u] < i)
                                {
                                    nodeAnomalies[u].push_back(v);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                for (auto v : candidates[i])
                {
                    if (!nodeBloom[v].isDefined())
                    {
                        pair<int, int> u = mate[v];
                        if (nodeOddLevel[u] < INFNITY)
                        {
                            int j = (nodeOddLevel[u] + nodeOddLevel[v]) / 2;
                            bridges[j].insert({u, v}); //wrong?
                        }
                        else if (nodeEvenLevel[u] == INFNITY)
                        {
                            nodePredecessors[u] = {v};
                            nodeSuccessors[v] = {u};
                            nodeCount[u] = 1;
                            nodeEvenLevel[u] = i + 1;
                            candidates[i + 1].push_back(u);
                        }
                    }
                }
            }

            for (auto pairs : bridges[i])
            {
                pair<int, int> s = pairs.first;
                pair<int, int> t = pairs.second;
                if (nodeErase[s] == UNERASED and nodeErase[t] == UNERASED)
                {
                    augmented = augmentBlossom(s, t, i);
                }
            }

            ++i;
        }
        return augmented;
    }

    bool augmentBlossom(pair<int, int> s, pair<int, int> t, int i)
    {
        bool foundBloom = false;
        bool augmented = false;

        pair<int, int> vL = nodeBloom[s].isDefined() ? baseStar(s) : s;
        pair<int, int> vR = nodeBloom[t].isDefined() ? baseStar(t) : t;

        if (vL == vR)
            return false;

        if (nodeBloom[s].isDefined())
            nodeParent[vL] = s;
        if (nodeBloom[t].isDefined())
            nodeParent[vR] = t;

        nodeMark[vL] = LEFT;
        nodeMark[vR] = RIGHT;
        bloomNodes.push_back(vL);
        bloomNodes.push_back(vR);

        DfsInfo dfsInfo(s, t, vL, vR, None, vR); //what is dcv aka none?

        while (!foundBloom and !augmented)
        {
            if (dfsInfo.vL == None or dfsInfo.vR == None)
                return false;
            int level_vL = min(nodeEvenLevel[dfsInfo.vL], nodeOddLevel[dfsInfo.vL]);
            int level_vR = min(nodeEvenLevel[dfsInfo.vR], nodeOddLevel[dfsInfo.vR]);

            if (mate.find(dfsInfo.vL) == mate.end() and mate.find(dfsInfo.vR) == mate.end())
            {
                vector<pair<int, int>> pathL = findPath(dfsInfo.s, dfsInfo.vL, none);
                vector<pair<int, int>> pathR = findPath(dfsInfo.t, dfsInfo.vR, none);
                vector<pair<int, int>> path = connectPath(pathL, pathR, dfsInfo.s, dfsInfo.t);
                augmentMatching(dfsInfo.vL, dfsInfo.vR);
                erasePath(path);
                augmented = true;
                break;
            }
            else if (level_vL >= level_vR)
            {
                foundBloom = leftDfs(dfsInfo);
            }
            else
            {
                foundBloom = rightDfs(dfsInfo);
            }
        }

        if (foundBloom and dfsInfo.dcv != None)
        {
            nodeMark[dfsInfo.dcv] = UNMARKED;
            Bloom b;
            b.peaks = {dfsInfo.s, dfsInfo.t};
            b.base = dfsInfo.dcv;
            b.setDefined();

            for (auto v : bloomNodes)
            {
                if (nodeMark[v] == UNMARKED or !nodeBloom[v].isDefined())
                    continue;
                nodeBloom[v] = b;

                int level_v = min(nodeEvenLevel[v], nodeOddLevel[v]);
                if (level_v % 2 == 0)
                    nodeOddLevel[v] = 2 * i + 1 - nodeEvenLevel[v];
                else
                {
                    nodeEvenLevel[v] = 2 * i + 1 - nodeOddLevel[v];
                    candidates[nodeEvenLevel[v]].push_back(v);
                    for (auto z : nodeAnomalies[v])
                    {
                        int j = (nodeEvenLevel[v] + nodeEvenLevel[z]) / 2;
                        bridges[j].insert({v, z}); //wrong?
                        edges[v][z]["use"] = USED;
                    }
                }
            }
        }
        bloomNodes.clear();
        return augmented;
    }

    vector<pair<int, int>> connectPath(vector<pair<int, int>> pathL, vector<pair<int, int>> pathR, pair<int, int> s, pair<int, int> t)
    {
        bool reverseL = s == pathL[0] ? true : false;
        bool reverseR = t == pathR[pathR.size() - 1] ? true : false;

        if (reverseL)
        {
            nodeParent[pathL[0]] = None;
            pair<int, int> prevv = None;
            pair<int, int> currentv = pathL[pathL.size() - 1];
            pair<int, int> nextv = None;
            while (currentv != None)
            {
                nextv = nodeParent[currentv];
                nodeParent[currentv] = prevv;
                prevv = currentv;
                currentv = nextv;
            }
            reverse(pathL.begin(), pathL.end());
        }

        if (reverseR)
        {
            nodeParent[pathR[0]] = None;
            pair<int, int> prevv = None;
            pair<int, int> currentv = pathR[pathR.size() - 1];
            pair<int, int> nextv = None;
            while (currentv != None)
            {
                nextv = nodeParent[currentv];
                nodeParent[currentv] = prevv;
                prevv = currentv;
                currentv = nextv;
            }
            reverse(pathR.begin(), pathR.end());
        }

        vector<pair<int, int>> path;
        path.insert(path.begin(), pathL.begin(), pathL.end());
        path.insert(path.end(), pathR.begin(), pathR.end());

        nodeParent[pathR[0]] = pathL[pathL.size() - 1];
        return path;
    }

    void augmentMatching(pair<int, int> lv, pair<int, int> rv)
    {
        pair<int, int> firstv = rv;
        pair<int, int> secondv = None;
        while (firstv != lv)
        {
            secondv = nodeParent[firstv];
            if (mate[secondv] != firstv)
            {
                mate[firstv] = secondv;
                mate[secondv] = firstv;
            }
            firstv = secondv;
        }
    }

    bool leftDfs(DfsInfo dfsInfo)
    {
        for (auto uL : nodePredecessors[dfsInfo.vL])
        {
            if (edges[dfsInfo.vL][uL]["use"] == USED or nodeErase[uL] == ERASED)
                continue;

            edges[dfsInfo.vL][uL]["use"] = USED;

            if (nodeBloom[uL].isDefined())
                uL = baseStar(uL);

            if (nodeMark[uL] == UNMARKED)
            {
                nodeMark[uL] = LEFT;
                nodeParent[uL] = dfsInfo.vL;
                dfsInfo.vL = uL;
                bloomNodes.push_back(uL);
                return false;
            }
            else if (uL == dfsInfo.vR)
            {
                dfsInfo.dcv = uL;
            }
        }

        if (dfsInfo.vL == dfsInfo.s)
            return true;
        else if (nodeParent[dfsInfo.vL] != None)
        {
            dfsInfo.vL = nodeParent[dfsInfo.vL];
        }

        return false;
    }

    bool rightDfs(DfsInfo dfsInfo)
    {
        for (auto uR : nodePredecessors[dfsInfo.vR])
        {
            if (edges[dfsInfo.vR][uR]["use"] == USED or nodeErase[uR] == ERASED)
                continue;

            edges[dfsInfo.vR][uR]["use"] = USED;

            if (nodeBloom[uR].isDefined())
                uR = baseStar(uR);

            if (nodeMark[uR] == UNMARKED)
            {
                nodeMark[uR] = RIGHT;
                nodeParent[uR] = dfsInfo.vR;
                dfsInfo.vR = uR;
                bloomNodes.push_back(uR);
                return false;
            }
            else if (uR == dfsInfo.vL)
            {
                dfsInfo.dcv = uR;
            }
        }

        if (dfsInfo.vR == dfsInfo.barrier)
        {
            dfsInfo.vR = dfsInfo.dcv;
            dfsInfo.barrier = dfsInfo.dcv;
            nodeMark[dfsInfo.vR] = RIGHT;
            if (nodeParent[dfsInfo.vL] != None)
            {
                dfsInfo.vL = nodeParent[dfsInfo.vL];
            }
            else if (nodeParent[dfsInfo.vR] != None)
            {
                dfsInfo.vR = nodeParent[dfsInfo.vR];
            }
        }

        return false;
    }

    void erasePath(vector<pair<int, int>> path)
    {
        while (path.size() > 0)
        {
            pair<int, int> y = path[path.size() - 1];
            path.pop_back();
            nodeErase[y] = ERASED;

            for (auto z : nodeSuccessors[y])
            {
                if (nodeErase[z] == UNERASED)
                    nodeCount[z] -= 1;

                if (nodeCount[z] == 0)
                    path.push_back(z);
            }
        }
    }

    vector<pair<int, int>> findPath(pair<int, int> high, pair<int, int> low, Bloom b)
    {
        int level_high = min(nodeEvenLevel[high], nodeOddLevel[high]);
        int level_low = min(nodeEvenLevel[low], nodeOddLevel[low]);
        if (high == low)
            return {high};

        vector<pair<int, int>> path;
        pair<int, int> v = high;
        pair<int, int> u = high;
        while (u != low)
        {
            bool hasUnvisitedPredecessor = false;

            for (auto p : nodePredecessors[v])
            {
                if (edges[p][v]["visit"] == UNVISITED)
                {
                    hasUnvisitedPredecessor = true;
                    if (!nodeBloom[v].isDefined() or (nodeBloom[v].peaks == b.peaks and nodeBloom[v].base == b.base))
                    {
                        edges[p][v]["visit"] = VISITED;
                        u = p;
                    }
                    else
                    {
                        u = nodeBloom[v].base;
                    }
                    break;
                }
            }

            if (!hasUnvisitedPredecessor)
            {
                v = nodeParent[v];
            }
            else
            {
                int level_u = min(nodeEvenLevel[u], nodeOddLevel[u]);
                if (nodeErase[u] == UNERASED and level_u >= level_low and (u == low or (nodeVisit[u] == UNVISITED and (nodeMark[u] == nodeMark[high] != UNMARKED or (!nodeBloom[u].isDefined() and (nodeBloom[u].peaks != b.peaks and nodeBloom[u].base != b.base))))))
                {
                    nodeVisit[u] = VISITED;
                    nodeParent[u] = v;
                    v = u;
                }
            }
        }

        while (u != high)
        {
            path.push_back(u);
            u = nodeParent[u];
            reverse(path.begin(), path.end());

            int j = 0;
            while (j <= path.size() - 1)
            {
                pair<int, int> xj = path[j];

                if (!nodeBloom[xj].isDefined() and (nodeBloom[xj].peaks != b.peaks and nodeBloom[xj].base != b.base))
                {
                    nodeVisit[xj] = UNVISITED;
                    vector<pair<int, int>> temp = openBloom(xj);
                    int pathLength = temp.size();
                    path.erase(path.begin() + j, path.begin() + j + 2);
                    path.insert(path.begin() + j, temp.begin(), temp.end());
                    nodeParent[xj] = j > 0 ? path[j - 1] : None;
                    j += pathLength - 1;
                }
                ++j;
            }
        }
        return path;
    }

    vector<pair<int, int>> openBloom(pair<int, int> x)
    {
        Bloom bloom = nodeBloom[x];
        pair<int, int> base = bloom.base;
        int level_x = min(nodeEvenLevel[x], nodeOddLevel[x]);
        vector<pair<int, int>> path;

        if (level_x % 2 == 0)
        {
            path = findPath(x, base, bloom);
        }
        else
        {
            pair<int, int> leftPeak = bloom.peaks.first;
            pair<int, int> rightPeak = bloom.peaks.second;

            if (nodeMark[x] == LEFT)
            {
                vector<pair<int, int>> pathLeft = findPath(leftPeak, x, bloom);
                vector<pair<int, int>>
                    pathRight = findPath(rightPeak, base, bloom);
                vector<pair<int, int>> path = connectPath(pathLeft, pathRight, leftPeak, rightPeak);
            }
            else if (nodeMark[x] == RIGHT)
            {
                vector<pair<int, int>> pathLeft = findPath(rightPeak, x, bloom);
                vector<pair<int, int>> pathRight = findPath(leftPeak, base, bloom);
                vector<pair<int, int>> path = connectPath(pathLeft, pathRight, rightPeak, leftPeak);
            }
        }
        return path;
    }

    pair<int, int> baseStar(pair<int, int> v)
    {
        pair<int, int> base = v;
        while (nodeBloom[base].isDefined())
        {
            base = nodeBloom[base].base;
        }
        return base;
    }
};