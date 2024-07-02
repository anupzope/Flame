#include <mixture.hh>

#include <gtest/gtest.h>

using namespace flame;

TEST(MixtureXMLParser, Mixture1) {
  Mixture mixture;
  std::ostringstream errmsg;
  int error = parseFromXML(std::string(FLAME_DATA_DIR)+"/mixture1.xml", mixture, errmsg);

  ASSERT_EQ(error, 0) << errmsg.str();
  ASSERT_EQ(mixture.nSpecies, 2);

  EXPECT_EQ(std::string(mixture.speciesName[0]), std::string("O2"));
  EXPECT_EQ(std::string(mixture.speciesName[1]), std::string("N2"));

  EXPECT_EQ(mixture.viscosityModel[0], VISCOSITY_SUTHERLAND);
  EXPECT_EQ(mixture.viscosityModel[1], VISCOSITY_SUTHERLAND);

  EXPECT_EQ(mixture.sutherlandViscosity[0].refViscosity, 1.919e-5);
  EXPECT_EQ(mixture.sutherlandViscosity[0].refTemperature, 273.0);
  EXPECT_EQ(mixture.sutherlandViscosity[0].refConstant, 139.0);

  EXPECT_EQ(mixture.sutherlandViscosity[1].refViscosity, 1.663e-5);
  EXPECT_EQ(mixture.sutherlandViscosity[1].refTemperature, 273.0);
  EXPECT_EQ(mixture.sutherlandViscosity[1].refConstant, 107.0);

  EXPECT_EQ(mixture.conductivityModel[0], CONDUCTIVITY_SUTHERLAND);
  EXPECT_EQ(mixture.conductivityModel[1], CONDUCTIVITY_SUTHERLAND);

  EXPECT_EQ(mixture.sutherlandConductivity[0].refConductivity, 0.0244);
  EXPECT_EQ(mixture.sutherlandConductivity[0].refTemperature, 273.0);
  EXPECT_EQ(mixture.sutherlandConductivity[0].refConstant, 240.0);

  EXPECT_EQ(mixture.sutherlandConductivity[1].refConductivity, 0.0242);
  EXPECT_EQ(mixture.sutherlandConductivity[1].refTemperature, 273.0);
  EXPECT_EQ(mixture.sutherlandConductivity[1].refConstant, 150.0);

  EXPECT_EQ(mixture.diffusivityModel[0], DIFFUSIVITY_SCHMIDT);
  EXPECT_EQ(mixture.diffusivityModel[1], DIFFUSIVITY_SCHMIDT);

  EXPECT_EQ(mixture.schmidtNumber[0].value, 0.11);
  EXPECT_EQ(mixture.schmidtNumber[1].value, 0.22);

  EXPECT_EQ(mixture.thermochemistryModel[0], THERMOCHEMISTRY_CALORICALLY_PERFECT);
  EXPECT_EQ(mixture.thermochemistryModel[1], THERMOCHEMISTRY_CALORICALLY_PERFECT);

  EXPECT_EQ(mixture.caloricallyPerfectThermochemistry[0].specificHeat, 920.0);
  EXPECT_EQ(mixture.caloricallyPerfectThermochemistry[1].specificHeat, 1005.0);
}
