/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 Dmitry "Dima" Korolev <dmitry.korolev@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include <vector>
#include <string>

#include "gnuplot.h"
#include "plotutils.h"

#include "../3party/gtest/gtest-main.h"

using namespace bricks::gnuplot;
using namespace bricks::plotutils;

TEST(Graph, GNUPlotSmokeTest) {
  const std::string result = GNUPlot()
                                 .Title("Foo 'bar' \"baz\"")
                                 .KeyTitle("Meh 'in' \"quotes\"")
                                 .XRange(-42, 42)
                                 .YRange(-2.5, +2.5)
                                 .Grid("back")
                                 .Plot([](Plotter& p) {
                                   for (int i = -100; i <= +100; ++i) {
                                     p(i, ::sin(0.1 * i));
                                   }
                                 })
                                 .Plot(WithMeta([](Plotter& p) {
                                                  for (int i = -100; i <= +100; ++i) {
                                                    p(i, ::cos(0.1 * i));
                                                  }
                                                })
                                           .Name("\"Cosine\" as 'points'")
                                           .AsPoints())
                                 .OutputFormat("svg");
  ASSERT_EQ(result, bricks::FileSystem::ReadFileAsString("golden/gnuplot.svg"));
}

TEST(Graph, PlotutilsLine) {
  const size_t N = 1000;
  std::vector<std::pair<double, double>> line(N);
  for (size_t i = 0; i < N; ++i) {
    const double t = M_PI * 2 * i / (N - 1);
    line[i] =
        std::make_pair(16 * pow(sin(t), 3), -(13 * cos(t) + 5 * cos(t * 2) - 2 * cos(t * 3) - cos(t * 4)));
  }
  const std::string result = Plotutils(line)
                                 .LineMode(CustomLineMode(LineColor::Red, LineStyle::LongDashed))
                                 .GridStyle(GridStyle::Full)
                                 .Label("Imagine all the people ...")
                                 .X("... living life in peace")
                                 .Y("John Lennon, \"Imagine\"")
                                 .LineWidth(0.015)
                                 .OutputFormat("svg");
  ASSERT_EQ(result, bricks::FileSystem::ReadFileAsString("golden/love.svg"));
}

TEST(Graph, PlotutilsPoints) {
  const int p1 = 27733;
  const int p2 = 27791;
  const double k = 1.0 / (p2 - 1);
  int tmp = 1;
  std::vector<std::pair<double, double>> flakes(200);
  for (auto& xy : flakes) {
    xy.first = k*(tmp = (tmp * p1) % p2);
    xy.second = k*(tmp = (tmp * p1) % p2);
  }
  const std::string result = Plotutils(flakes)
                                 .LineMode(LineMode::None)
                                 .GridStyle(GridStyle::None)
                                 .Symbol(Symbol::Asterisk, 0.1)
                                 .OutputFormat("svg");
  ASSERT_EQ(result, bricks::FileSystem::ReadFileAsString("golden/flakes.svg"));
}

TEST(Graph, PlotutilsMultiplot) {
  auto gen = [](int phi) {
    std::vector<std::pair<double, double>> xy(4);
    for (int i = 0; i <= 3; ++i) {
      const double alpha = (phi + i * 120) * M_PI / 180;
      xy[i] = std::make_pair(sin(alpha), cos(alpha));
    }
    return xy;
  };
  const std::string result =
      Plotutils({gen(0), gen(60)}).LineWidth(0.005).GridStyle(GridStyle::None).OutputFormat("svg");
  ASSERT_EQ(result, bricks::FileSystem::ReadFileAsString("golden/david.svg"));
}