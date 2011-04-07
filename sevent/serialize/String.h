#pragma once

namespace sevent
{
    namespace serialize
    {
        typedef boost::shared_ptr<std::string> String_ptr;
        serialize::BaseResult_ptr serializeString(boost::any& data)
        {
            String_ptr s = boost::any_cast<String_ptr>(data);
            return boost::make_shared<serialize::ConstPtrResult>(s->c_str(), s->size());
        }

        boost::any deserializeString(datastruct::MutableCharArray_ptr serialized)
        {
            String_ptr s = boost::make_shared<std::string>(serialized->data);
            return s;
        }
        serialize::Pair String(serializeString,
                               deserializeString);
    } // namespace serialize
} // namespace sevent
