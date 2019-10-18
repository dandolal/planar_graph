
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <list>

struct arc {
  arc(int f, int t) {
    from = f;
    to = t;
  }

  int from, to;
};

class Graph {
 public:
  Graph(int n) : vertices(n, false), neighbors(n) {};

  void add_vertex(int vertex) {
    vertices[vertex] = true;
    empty = false;
  }

  void add_arc(int from, int to) {
    arcs.emplace_back(arc(from, to));
    neighbors[from].push_back(to);
    neighbors[to].push_back(from);
    add_vertex(from);
    add_vertex(to);
    empty = false;
  }

  int size() const {
    return vertices.size();
  }

  void print() const {
    for (int i = 0; i < vertices.size(); ++i) {
      if (vertices[i])
        std::cout << i << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < arcs.size(); ++i)
      std::cout << arcs[i].from << "-" << arcs[i].to << " ";
    std::cout << std::endl;
  }

  bool check_vertex(int vertex) const {
    return vertices[vertex];
  }

  bool check_arc(int from, int to) const {
    for (auto arc: arcs)
      if (from == arc.from && to == arc.to || to == arc.from && from == arc.to)
        return true;
    return false;
  }
  bool is_empty() const {
    return  empty;
  }

  void get_neighbors(int v, std::vector<int>& vert) const {
    vert = neighbors[v];
  }

  void get_arcs (std::vector<arc>& a) const {
    a = arcs;
  }


 private:
  std::vector<std::vector<int>> neighbors;
  std::vector<arc> arcs;
  bool empty = true;
  std::vector<bool> vertices;


};

class Edge{
 public:
  void add_vertex(int vertex) {
    vertices.push_back(vertex);
  }
  std::vector<int> vertices;
 private:

};

bool compare(std::pair<Graph, int> p1, std::pair<Graph, int> p2){
  return p1.second < p2.second;
}




void find_circle(Graph g, std::list<std::pair<Graph, int>> &parts, Graph &res) {
  std::stack<int> vertices;
  std::vector<bool> used(g.size(), false);
  std::vector<int> prev(g.size(), -1);
  int point;
  vertices.push(0);
  bool mark = false;
  while (!vertices.empty()) {
    int vertex = vertices.top();
    vertices.pop();

    used[vertex] = true;
    std::vector<int> neighbors;
    g.get_neighbors(vertex, neighbors);
    for (auto v: neighbors) {
      if (v != prev[vertex]) {
        prev[v] = vertex;
        vertices.push(v);
        if (used[v]) {
          point = v;
          mark = true;
          break;
        }
      }

    }
    if (mark)
      break;
  }
  int from = point;
  int to = prev[point];
  while (to != point){
    res.add_arc(from, to);
    from = to;
    to = prev[to];
  }
  res.add_arc(from, to);
//    res.print();
}

bool check_if_tree(Graph g) {
  std::queue<int> vertices;
  std::vector<bool> used(g.size(), false);
  std::vector<int> prev(g.size(), -1);
  for (int i = 0; i < g.size(); ++i) {
    if (g.check_vertex(i)) {
      vertices.push(i);
      while (!vertices.empty()) {
        int vertex = vertices.front();
        if (used[vertex])
          return false;
        used[vertex] = true;
        std::vector<int> neighbors;
        g.get_neighbors(vertex, neighbors);
        for (auto v: neighbors) {
          if (v != prev[vertex]) {
            prev[v] = vertex;
            vertices.push(v);
          }
        }
        vertices.pop();
      }
      break;
    }
  }
  return true;
}



void split_components(std::queue<Graph> &components) {
  Graph g = components.front();
  int i = 0;
  std::vector<bool> used(g.size(), false);
  while (true) {
    if (i >= g.size())
      break;
    if (used[i])
      ++i;
    else {
      Graph g1(g.size());
      std::queue<int> vertices;
      vertices.push(i);
      while (!vertices.empty()) {
        int vertex = vertices.front();
        used[vertex] = true;
        std::vector<int> neighbors;
        g.get_neighbors(vertex, neighbors);
        for (auto v: neighbors) {
          if (!g1.check_arc(v, vertex))
            g1.add_arc(v, vertex);
          if (!used[v]) {
            vertices.push(v);
          }
        }
        vertices.pop();
      }
      components.push(g1);
      ++i;

    }
  }
  components.pop();
}

void find_route(Graph& g, int from, int to, Graph& route){
  std::stack<int> vertices;
  std::vector<bool> used(g.size(), false);
  std::vector<int> prev(g.size(), -1);
  vertices.push(to);
  bool mark = false;
  while (!vertices.empty()) {
    int vertex = vertices.top();
    vertices.pop();
    used[vertex] = true;
    std::vector<int> neighbors;
    g.get_neighbors(vertex, neighbors);
    for (auto v: neighbors) {
      if (!used[v]) {
        prev[v] = vertex;
        vertices.push(v);
      }
      if (v == from){
        mark = true;
      }
    }
    if (mark)
      break;
  }
  int point = to;
  while(from != point){
    to = prev[from];
    route.add_arc(from, to);
    from = to;
  }
}

int num_same_vert(Graph& g, Graph& res){
  int num = 0;
  for (int i = 0; i < g.size(); ++i)
    if (g.check_vertex(i) && res.check_vertex(i))
      ++num;
  return  num;
}

bool split_into_parts(Graph& g, Graph& route, std::list<std::pair<Graph, int>>& parts, Graph& res, std::vector<std::vector<int>>& edges){
  std::vector<bool> used(g.size(), false);
  std::vector<int> no_vertices_route;
  std::queue<int> qvertices;

  for (int i = 0; i < route.size(); ++i)
    if (!res.check_vertex(i) && g.check_vertex(i))
      no_vertices_route.push_back(i);
  for (int i: no_vertices_route){
    if(!used[i]){

      used[i] = true;

      qvertices.push(i);

      Graph part(g.size());
      while (!qvertices.empty()){
        int vertex = qvertices.front();
        std::vector<int> neighbors;
        g.get_neighbors(vertex, neighbors);
        for (auto v: neighbors) {
          if (!part.check_arc(v, vertex)) {
            part.add_arc(vertex, v);
            if (!res.check_vertex(v)) {
              qvertices.push(v);
              used[v] = true;
            }
          }
        }
        qvertices.pop();
      }
      if (!part.is_empty()){
        std::vector<int> num_v(g.size(), 0);
        for (int i = 0; i < g.size(); ++i){
          if (res.check_vertex(i) && part.check_vertex(i))
            for (int edge: edges[i])
              ++num_v[edge];
        }
        int count = 0;
        int svert = num_same_vert(res, part);
        for (int n: num_v)
          if (n == svert)
            ++count;
        parts.emplace(parts.end(), part, count);
        if (count == 0)
          return false;
      }


    }
  }
  std::queue<int> vertices_route;
  for (int i = 0; i < route.size(); ++i){
    if (route.check_vertex(i))
      vertices_route.push(i);
  }
  while (!vertices_route.empty()){
    int vertex = vertices_route.front();
    used[vertex] = true;
    std::vector<int> neighbors;
    g.get_neighbors(vertex, neighbors);
    for (auto v: neighbors)
      if (!used[v] && !res.check_arc(v, vertex)){
        Graph part(g.size());
        part.add_arc(vertex, v);
        int count = 0;
        for (int i: edges[v])
          for (int j: edges[vertex])
            if (i == j)
              ++count;
        if (count == 0)
          return false;
        parts.emplace(parts.end(), part, count);
      }
    vertices_route.pop();
  }
  parts.pop_front();
  return true;
}

bool check_planarity(Graph g);

bool find_chain(Graph& res, Graph& part, int edge_number, std::vector<std::vector<int>>& edges, int& from, int& to, int& good_edge){
  from = -1;
  to = -1;
  std::vector<int> num_edge(edge_number, 0);
  for(int i = 0; i < part.size(); ++i)
    if (part.check_vertex(i)){
      for (auto edge: edges[i])
        ++num_edge[edge];
    }
  int sum = 0;
  for (int i = 0; i < num_edge.size(); ++i)
    sum += num_edge[i];
  if (sum == 1)
    return check_planarity(part);
  good_edge = -1;
  for (int i = 0; i < num_edge.size(); ++i)
    if (num_edge[i] >= 2){
      good_edge = i;
      break;
    }
  if (good_edge == -1)
    return false;
  for(int i = 0; i < part.size(); ++i)
    if (part.check_vertex(i)){
      for (auto edge: edges[i])
        if (edge == good_edge){
          from = i;
          break;
        }
      if (from != -1)
        break;
    }
  for(int i = 0; i < part.size(); ++i)
    if (part.check_vertex(i)){
      for (auto edge: edges[i])
        if (edge == good_edge && from != i){
          to = i;
          break;
        }
      if (to != -1)
        break;
    }
  return true;
}

bool add_part(std::list<std::pair<Graph, int>> &parts, Graph &res, int& edge_number, std::vector<std::vector<int>>& edges, std::vector<Edge>& plane) {
  int from;
  int to;
  bool mark = true;
  int good_edge;

  mark = find_chain(res, parts.front().first, edge_number, edges, from, to, good_edge);
  if (!mark)
    return mark;
  Graph route(res.size());
  find_route(parts.front().first, from, to, route);

  std::vector<int> num_v(parts.front().first.size(), 0);
  for (int i = 0; i < parts.front().first.size(); ++i){
    if (res.check_vertex(i) && parts.front().first.check_vertex(i))
      for (int edge: edges[i])
        ++num_v[edge];
  }
  int count = 0;
  int svert = num_same_vert(res, parts.front().first);
  for (int n: num_v) {
    if (n == svert)
      good_edge = count;
    ++count;
  }

  std::vector<int> route_vertices;
  std::vector<arc> arcs;
  route.get_arcs(arcs);
  route_vertices.push_back(arcs[0].from);
  for (arc a: arcs) {
    route_vertices.push_back(a.to);
    res.add_arc(a.from, a.to);
  }

  std::vector<int> edge1;
  std::vector<int> edge2;
  int i = 0;
  while (true){
    if (plane[good_edge].vertices[i] == from || plane[good_edge].vertices[i] == to)
      break;
    edge1.push_back(plane[good_edge].vertices[i]);
    ++i;
  }
  if (plane[good_edge].vertices[i] == to)
    std::reverse(route_vertices.begin(), route_vertices.end());
  for (int vertex: route_vertices)
    edge1.push_back(vertex);
  ++i;
  while (true){
    if (plane[good_edge].vertices[i] == from || plane[good_edge].vertices[i] == to)
      break;
    edge2.push_back(plane[good_edge].vertices[i]);
    for (int j = 0; j < edges[plane[good_edge].vertices[i]].size(); ++j){
      if (edges[plane[good_edge].vertices[i]][j] == good_edge)
        edges[plane[good_edge].vertices[i]][j] = edge_number;
    }
    ++i;

  }
  std::reverse(route_vertices.begin(), route_vertices.end());
  for (int vertex: route_vertices) {
    edge2.push_back(vertex);
    edges[vertex].push_back(edge_number);
    edges[vertex].push_back(good_edge);
  }
  edges[from].pop_back();
  edges[to].pop_back();
  ++i;
  while (i < route.size()){
    edge1.push_back(plane[good_edge].vertices[i]);
    ++i;
  }
  ++edge_number;
  plane[good_edge].vertices = edge1;
  Edge e;
  e.vertices = edge2;
  plane.push_back(e);
  mark = split_into_parts(parts.front().first, route, parts, res, edges);
  if (!mark)
    return false;
  return true;
}



bool check_planarity(Graph g) {
  if (check_if_tree(g))
    return true;
  bool mark = true;
  std::vector<Edge> plane;
  std::list<std::pair<Graph, int>> parts;
  std::vector<std::vector<int>> edges(g.size());
  Graph res(g.size());
  int edge_number = 2;
  find_circle(g, parts, res);
  //res.print();
  parts.emplace(parts.end(), res, num_same_vert(res, res));
  for (int i = 0; i < res.size(); ++i){
    if (res.check_vertex(i)){
      edges[i].push_back(0);
      edges[i].push_back(1);
    }
  }
  plane.emplace_back(Edge());
  plane.emplace_back(Edge());
  std::vector<arc> arcs;
  res.get_arcs(arcs);
  for (int i = 0; i < arcs.size(); ++i){
    plane[0].add_vertex(arcs[i].from);
    plane[1].add_vertex(arcs[i].from);
  }


  mark = split_into_parts(g, res, parts, res, edges);
//    while (!parts.empty())
//        mark = add_part(parts, res);

  while(!parts.empty()) {
    if (num_same_vert(res, parts.front().first) == 1) {
      Graph g = parts.front().first;
      parts.pop_front();
      if(!check_planarity(g))
        return false;
    }
    parts.sort(compare);
    mark = add_part(parts, res, edge_number, edges, plane);
    if (!mark)
      return mark;
  }
  return mark;
}

int main() {
  bool mark = true;
  int nver;
  std::cout << "Enter the number of vertices\n";
  std::cin >> nver;
  int narcs;
  std::cout << "Enter the number of edges\n";
  std::cin >> narcs;
  Graph g(nver);
  int from, to;
  std::cout << "Enter pairs of vertices, which are connected with edge. Vertex numbering starts with 0\n";
  for (int i = 0; i < narcs; ++i) {
    std::cin >> from >> to;
    g.add_arc(from, to);
  }
  std::queue<Graph> components;
  components.push(g);
  split_components(components);
  while (!components.empty()) {
    mark = check_planarity(components.front());
    if (!mark){
      std::cout << "This graph is not planar";
      return 0;
    }
    components.pop();
  }
  std::cout << "This graph is planar";
  return 0;
}
