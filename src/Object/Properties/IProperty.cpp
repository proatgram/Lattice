module Lattice.Object.Properties.IProperty;

using namespace Lattice::Object::Properties;

IProperty::IProperty(Visibility visibility) : m_visibility(visibility) {};

auto IProperty::GetVisibility() const -> Visibility {
    return m_visibility;
}
