�ֹ� ó�� ����. 
���� ���� �ϴ� �Լ��� �� �Ѱ��� �����ϵ��� �Ѵ�. 
SI ������ �ֹ��� �����ϰų� ó���� �� �׻� ���ο� �ֹ� ��ȣ�� �´�. 
�׸��� ����Ȯ���̳� ���Ȯ�� �ֹ� ��ȣ�� ���߿� �´�. 
�� �� ������ �ֹ��̳� ��ҵ� �ֹ��� Ȯ���Ͽ� ����� ��� �Ѵ�. 
��ҳ� ������ �� �� ���� ȭ�鿡�� ���ְ� ���Ȯ���̳� ����Ȯ���� ���߿� ó���ϴ� ������ �Ѵ�. 
�ֹ��� �� ���� �ֹ� ��ü�� ���� ����� ���� ȭ�鿡 ǥ���ϰ� �ֹ� ���¸� Ledge���·� �����Ѵ�.
Ledge������ �ֹ��� ����ϰų� ������ �� ����. 

�ֹ� Ȯ�� ó�� ����.
1. ���ǻ翡�� �ֹ� ������ �޴´�.
2. �ֹ������� �Ľ��Ͽ� order_info json ��ü�� �����.
3. order_processor ť�� �ִ´�.
4. order_processor�� ť���� ������ �ֹ��� ó���ϱ� �����Ѵ�.
5. order_info�� ���� ���Ŀ� order_request_id�� �ִ��� �ľ��Ѵ�.
6. order_request_id  ������ �ִ� ��� 
  6.1 order_request_list���� order_request�� ã�� ����. 
  6.2 order_request_id�� �ִ� ���
    6.2.1 order_request�� �������� order��ü�� �����.
    6.2.2 order�� order_id, accepted_count, accepted_time�� �־��ش�. 
7. order_request_id ������ ���� ��� - ������ �ܺ� �ֹ�(���� ����ڰ� �ֹ��� ���� �ֹ�â �ܿ� �ٸ� process, �ٸ� ip�ּ� �� �ֹ�)���� ����
  7.1 order_info�� �������� order ��ü�� �����. 
  7.2 order�� account_no, symbol_code, order_id, accepted_count, accepted_time�� �־��ش�.
8. symbol_order_manager accepted_order_list�� �߰��� �ش�. 
9. symbol_order_manager buy_order_manager/sell_order_manager�� order�� �߰��� �ش�. 
10. account_accepted_order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 
11. order_view�� accepted_order_list�� ���ŵǾ��ٴ� event�� ������. 

