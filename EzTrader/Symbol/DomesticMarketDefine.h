#pragma once
#include <string>
#include <memory>
namespace DarkHorse {
	const std::string DmFutureMarketName = "��������";
	const std::string DmOptionMarketName = "�����ɼ�";
	class SmProduct;
struct DmFuture {
	std::string future_name;
	std::string product_code;
	std::shared_ptr<SmProduct> product;
};
struct DmOption {
	std::string option_name;
	std::shared_ptr<SmProduct> call_product;
	std::shared_ptr<SmProduct> put_product;
};
}